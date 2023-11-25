#include "photon_mapping.hpp"

using namespace PhotonMapping;

void castPhotonToScene(const ObjectSet& objSet, const Ray& ray,
        Photon photon, std::vector<Photon>& photonRegister,
        Randomizer& random, Index total, Index& mapped)
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
            castPhotonToScene(objSet, secondaryRay, photon, photonRegister,
                    random, total, mapped);
            break;
        case Material::Component::kd:

            Real lat = std::acos(ray.d[0]);
            if (ray.d[2] < 0)
                lat = -lat;

            Real az = std::acos(ray.d[1] / std::sin(lat));
            if (ray.d[0] < 0)
                az = -az;

            photonRegister.emplace_back(hit, photon.flux * color, lat, az);

            photon.flux = photon.flux * color;
            mapped++;
            castPhotonToScene(objSet, secondaryRay, photon, photonRegister,
                    random, total, mapped);
            break;
    }
}

Color castRayToScene(const ObjectSet& objSet, const Ray& ray,
        const PhotonMap& map, Randomizer& random)
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
    //const auto normal = shape.normal(ray.d, hit);

    // Ray secondaryRay;
    // const auto [color, k] = material.eval<false>(hit, ray, secondaryRay, normal, random);

    const Real radius = 0.05;
    const Index photons = 10000;
    auto nearest = map.nearest_neighbors(hit, photons, radius); // Preguntar por estos valores
    
    // uniform kernel
    Color sum;
    for (const Photon* photon : nearest)
        sum = sum + photon->flux * material.kd(); //divide by pi ^2??

    return sum / (radius * radius);
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

void PhotonMapping::Renderer::
render(const Camera& camera, Image& img, const ObjectSet& objects,
        Index ppp, Index totalPhotons)
{
    constexpr std::string_view jump_to_previous_line = "\033[F";

    std::cout << "Algorithm: photon mapping\n";
    std::cout << "Worker pool size: " << 1/* numThreads()*/ << "\n\n";

    TextProgressBar progressBar;

    std::cout << "Casting photons into the scene...\n";
    progressBar.launch();

    const auto photonsPerLight = dividePhotonsByPower(objects, totalPhotons);

    Index photonsInList = 0;
    std::vector<Photon> photonList(totalPhotons);

    Randomizer random {0.0, 1.0};
    for (Index i : numbers::range(0, photonsPerLight.size()))
    {
        const auto& light = objects.pointLights[i];
        const Index numPhotons = photonsPerLight[i];

        Index mapped = 0, casted = 0;
        std::vector<Photon> lightRegister;
        Photon photon;

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
            Index preMapped = mapped;
            castPhotonToScene(objects, ray, photon, lightRegister, random, numPhotons, mapped);

            if (preMapped < mapped)
            {
                casted++;
                progressBar.incrementProgress(Real(mapped - preMapped) / totalPhotons);
            }
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
    PhotonMap map {photonList, Photon::KDTreeAccessor{}};
    std::vector<Photon>().swap(photonList);

    std::cout << jump_to_previous_line;
    std::cout << "Creating photon map: done ✔️ \n";

    std::cout << "Reading light from the scene...\n";
    std::cout.flush();
    progressBar.launch();
    
    const Real increment = 1.0 / img.pixels();

    Camera cam {camera};
    const auto [width, height] = img.dimensions();
    for (Index i : numbers::range(0, height)) //for i = start.i .. end.i
    {
        for (Index j : numbers::range(0, width))
        {
            Color meanColor {0, 0, 0};
            for ([[maybe_unused]] Index k : numbers::range(0, ppp))
            {
                Ray ray = cam.randomRay(i, j);

                meanColor = meanColor + castRayToScene(objects, ray, map, random);
            }
            // Thread-safe operation: a pixel is not assigned to two different threads 
            img(i, j) = RGBPixel (meanColor / ppp);

            progressBar.incrementProgress(increment);
        }
    }

    progressBar.stop(true);
    progressBar.join();
    
    std::cout << jump_to_previous_line;
    std::cout << "Reading light from the scene: done ✔️ \n";

    img.updateLuminance();

    std::cout << std::endl;
}
