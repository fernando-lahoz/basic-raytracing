#include "photon_mapping.hpp"

using namespace PhotonMapping;

bool TaskDivider:: getNextTask(Task& task)
{
    if (j == width)
    {
        j = 0;
        i = numbers::min(i + regionHeight, height);
        if (i == height)
            return false;
    }
    
    task.start = {i, j};
    j = numbers::min(j + regionWidth, width);
    task.end = {numbers::min(i + regionHeight, height), j};

    return true;
}

PhotonMapping::Renderer::
Renderer(const Index numWorkers, const Index queueSize,
        const TaskDivider& divider)
    : tasks{queueSize}, taskDivider{divider}
{
    auto hw = std::thread::hardware_concurrency();
    const Index nThreads = numWorkers <= 0
            ? numbers::max(hw, (decltype(hw))1)
            : numWorkers;

    threadPool.resize(nThreads);
}

Index PhotonMapping::Renderer::
numThreads()
{
    return threadPool.size();
}

void leaderRoutine(TaskQueue& tasks, TaskDivider& divider)
{
    Task task;
    while (divider.getNextTask(task))
    {
        tasks.enqueue(task);
    }
    tasks.stop(); // Tell threads not to block if queue is empty, but to quit
}

template<typename PhotonTy>
void castPhotonToScene(const ObjectSet& objSet, const Ray& ray,
        PhotonTy photon, std::vector<PhotonTy>& photonRegister,
        Randomizer& random, Index total, Index& mapped, bool save)
{
    if (mapped == total)
        return;
    
    const auto [t, hitObj] = findIntersection(objSet, ray);

    if (!Ray::isHit(t))
        return;

    const auto& shape = hitObj->shape();
    const auto& material = hitObj->material();

    // if emits ?? -> shouldn't be any area light

    const auto hit = ray.hitPoint(t);
    const auto normal = shape.normal(ray.d, hit);

    Ray secondaryRay;
    const auto [color, k] = material.eval(hit, ray, secondaryRay, normal, random);

    switch(k)
    {
    case Material::Component::ka:
        break; // don't save and leave
    case Material::Component::ks:
    case Material::Component::kt:
        castPhotonToScene<PhotonTy>(objSet, secondaryRay, photon,
                photonRegister, random, total, mapped, true);
        break;
    case Material::Component::kd:
        if (save)
        {
            Real lat = std::acos(ray.d[0]);
            if (ray.d[2] < 0)
                lat = -lat;

            Real az = std::acos(ray.d[1] / std::sin(lat));
            if (ray.d[0] < 0)
                az = -az;
            
            if constexpr (std::same_as<PhotonTy, SPhoton>)
                photonRegister.emplace_back(hit, photon.flux * color, lat, az, &shape);
            else
                photonRegister.emplace_back(hit, photon.flux * color, lat, az);
            mapped++;
        }

        photon.flux = photon.flux * color;
        castPhotonToScene<PhotonTy>(objSet, secondaryRay, photon, photonRegister,
                random, total, mapped, true);
        break;
    }
}

template<typename PhotonTy>
Color castRayToScene(const ObjectSet& objSet, const Ray& ray,
        const PhotonMap<PhotonTy>& map, Randomizer& random, Real radius, Index numPhotons, bool nextEvent)
{
    const auto [t, hitObj] = findIntersection(objSet, ray);

    if (!Ray::isHit(t))
        return Color{};

    const auto& shape = hitObj->shape();
    const auto& material = hitObj->material();

    // Shouldn't be any area light
    // const auto [emits, emission] = material.emission();
    // if (emits)
    //     return emission;

    const auto hit = ray.hitPoint(t);
    const auto normal = shape.normal(ray.d, hit);

    const auto [rd, rs, rt, pd, ps, pt] = material.sampleAll(hit, ray, normal, random);

    if (normal.side == Shape::Side::in) //parche imperfecto -> usar puntero a figura o mejor kt?? + parchear en path tracing
    {
        return castRayToScene<PhotonTy>(objSet, rt, map, random, radius, numPhotons, nextEvent);
    }

    Color cd, cs, ct;
    if (ps > 0.001)
        cs = castRayToScene<PhotonTy>(objSet, rs, map, random, radius, numPhotons, nextEvent);
    if (pt > 0.001)
        ct = castRayToScene<PhotonTy>(objSet, rt, map, random, radius, numPhotons, nextEvent);

    if (pd > 0.001)
    {
        auto nearest = map.nearest_neighbors(hit, numPhotons, radius);
        Index count = 0;

        Real sumOfWeights = 0;
        for (const PhotonTy* photon : nearest)
        {
            const Real r = norm(hit - photon->position);
            const Real weight = (radius - r) / radius;
            sumOfWeights += weight;
        }
        
        //std::cout << "sumOfWeights: " << sumOfWeights << '\n';

        for (const PhotonTy* photon : nearest)
        {
            if constexpr (std::same_as<PhotonTy, SPhoton>)
                if (photon->shape != &shape)
                    continue;
#if 0
//UNIFORM KERNEL
            cd = cd + photon->flux * material.kd();
            count++;
        }
        cd = cd / (radius * radius * numbers::pi * numbers::pi);
#elif 1
//CONE KERNEL

            const Real r = norm(hit - photon->position);
            const Real weight = (radius - r) / (radius);
            cd = cd + photon->flux * material.kd() * weight;
            count++;
        }
        
        cd = cd / (radius * radius * radius * numbers::pi * numbers::pi);// * (nearest.size() / count);
        //std::cout << "cd: " << cd << '\n';
#elif 1
//GAUSSIAN KERNEL

            const Real r = norm(hit - photon->position);
            const Real weight = (radius - r) / (radius);
            cd = cd + photon->flux * material.kd() * weight;
            count++;
        }
        
        cd = cd / (radius * radius * radius * numbers::pi * numbers::pi);// * (nearest.size() / count);
        //std::cout << "cd: " << cd << '\n';

#endif
        if (nextEvent) 
            cd = cd + castShadowRays(objSet, normal.normal, hit, material.kd());
    }
    
    return (cd * pd) + (cs * ps) + (ct * pt);
}

template<typename PhotonTy>
void workerRoutine(TaskQueue& tasks, Real increment, const Camera& camera,
        Image& img, const ObjectSet& objects, Index ppp, Real radius, Index numPhotons,
        const PhotonMap<PhotonTy>& map, TextProgressBar& progressBar,
        bool nextEvent)
{
    Camera cam {camera};
    Randomizer random {0.0, 1.0};
    Task task {};

    while (tasks.dequeue(task))
    {
        for (Index i : numbers::range(task.start.i, task.end.i)) //for i = start.i .. end.i
        for (Index j : numbers::range(task.start.j, task.end.j))
        {
            Color meanColor {0, 0, 0};
            for ([[maybe_unused]] Index k : numbers::range(0, ppp))
            {
                Ray ray = cam.randomRay(i, j);
                meanColor = meanColor
                          + castRayToScene<PhotonTy>(objects, ray, map, random,
                                                     radius, numPhotons, nextEvent);
            }
            // Thread-safe operation: a pixel is not assigned to two different threads 
            img(i, j) = RGBPixel (meanColor / ppp);
        }
        progressBar.incrementProgress(increment);
    }
}

std::vector<Index> dividePhotonsByPower(const ObjectSet& objSet, Index total)
{
    const auto& lights = objSet.pointLights;
    const Index numLights = lights.size();
    std::vector<Index> photonsPerLight(numLights);

    Real sumOfPower = 0;
    for (const auto& light : lights)
        sumOfPower += light.color().luminance();
    
    Integer sum = 0;
    for (Index i : numbers::range(0, numLights))
    {
        photonsPerLight[i] = lights[i].color().luminance() / sumOfPower;
        sum += photonsPerLight[i];
    }
        
    Integer remaining = total - sum;
    if (remaining > 0)
    {
        for (auto& num : photonsPerLight)
            num += remaining / numLights;

        photonsPerLight[numLights - 1] += remaining % numLights;
    }
    else if (remaining < 0)
    {
        remaining = -remaining;
        for (auto& num : photonsPerLight)
            num -= remaining / numLights;

        photonsPerLight[numLights - 1] -= remaining % numLights;
    }

    return photonsPerLight;
}   

template<typename PhotonTy>
void PhotonMapping::Renderer::
renderSpecialized(const Camera& cam, Image& img, const ObjectSet& objects,
        Index ppp, Index totalPhotons, Real evalRadius, Index evalNumPhotons, bool nextEventEstimation)
{
    constexpr std::string_view jump_to_previous_line = "\033[F";

    std::cout << "Algorithm: photon mapping\n";
    std::cout << "Worker pool size: " << numThreads() << "\n\n";

    TextProgressBar progressBar;

    std::cout << "Casting photons into the scene...\n";
    progressBar.launch();

    const auto photonsPerLight = dividePhotonsByPower(objects, totalPhotons);

    Index photonsInList = 0;
    std::vector<PhotonTy> photonList(totalPhotons);

    Randomizer random {0.0, 1.0};
    for (Index i : numbers::range(0, photonsPerLight.size()))
    {
        const auto& light = objects.pointLights[i];
        const Index numPhotons = photonsPerLight[i];

        Index mapped = 0, casted = 0;
        std::vector<PhotonTy> lightRegister;
        PhotonTy photon;

        while (mapped < numPhotons)
        {
            photon.flux = 4 * numbers::pi * light.color();

            const Real cosLat = 2 * random() - 1;
            photon.lat = std::acos(cosLat);
            const Real sinLat = std::sin(photon.lat);
            photon.az = 2 * numbers::pi * random();

            const Direction dir { sinLat * std::sin(photon.az),
                                  cosLat,
                                  sinLat * std::cos(photon.az)};
            
            Ray ray {light.position(), dir};
            Real preMapped = mapped;
            castPhotonToScene<PhotonTy>(objects, ray, photon, lightRegister,
                    random, numPhotons, mapped, !nextEventEstimation);

            casted++;
            progressBar.incrementProgress(Real(mapped - preMapped) / totalPhotons);
        }

        // Maybe this is fine   ???
        for (auto& p : lightRegister)
            p.flux = p.flux / casted;
        
        std::copy(lightRegister.begin(), lightRegister.end(),
                photonList.begin() + photonsInList);
        
        photonsInList += mapped;
    }

    progressBar.stop(true);
    progressBar.join();
    
    std::cout << jump_to_previous_line;
    std::cout << "Casting photons into the scene: done ✔️ \n";

    std::cout << "Creating photon map...\n";
    PhotonMap<PhotonTy> map {photonList, {}};
    std::vector<PhotonTy>().swap(photonList);

    std::cout << jump_to_previous_line;
    std::cout << "Creating photon map: done ✔️ \n";

    std::cout << "Reading light from the scene...\n";
    std::cout.flush();
    progressBar.launch();
    
    const Real totalSize = taskDivider.width * taskDivider.height;
    const Real regionSize = taskDivider.regionWidth * taskDivider.regionHeight;
    const Real increment = regionSize / totalSize;

    leader = std::thread(leaderRoutine, 
            std::ref(tasks), std::ref(taskDivider));
    for (auto& worker : threadPool)
    {
        worker = std::thread(workerRoutine<PhotonTy>, std::ref(tasks), increment,
                std::cref(cam), std::ref(img), std::cref(objects), ppp,
                evalRadius, evalNumPhotons, std::cref(map),
                std::ref(progressBar), nextEventEstimation);
    }

    leader.join();
    for (auto& worker : threadPool)
        worker.join();

    progressBar.stop(true);
    progressBar.join();
    
    std::cout << jump_to_previous_line;
    std::cout << "Reading light from the scene: done ✔️ \n";

    img.updateLuminance();

    std::cout << std::endl;
}

void PhotonMapping::Renderer::
render(const Camera& cam, Image& img, const ObjectSet& objects,
        Index ppp, Index totalPhotons, Real evalRadius, Index evalNumPhotons, bool nextEventEstimation,
        bool onlyCountSameShapePhotons)
{
    if (onlyCountSameShapePhotons)
    {
        renderSpecialized<SPhoton>(cam, img, objects,
                ppp, totalPhotons, evalRadius, evalNumPhotons, nextEventEstimation);
    }
    else
    {
        renderSpecialized<Photon>(cam, img, objects,
                ppp, totalPhotons, evalRadius, evalNumPhotons, nextEventEstimation);
    }
}
