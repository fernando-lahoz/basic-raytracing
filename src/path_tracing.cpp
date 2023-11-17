#include "path_tracing.hpp"

bool TaskDivider::getNextTask(Task& task)
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

PathTracingThreadPool::
PathTracingThreadPool(const Index numWorkers, const Index queueSize,
        const TaskDivider& divider)
    : tasks{queueSize}, taskDivider{divider}
{
    auto hw = std::thread::hardware_concurrency();
    const Index nThreads = numWorkers <= 0
            ? numbers::max(hw, (decltype(hw))1)
            : numWorkers;

    threadPool.resize(nThreads);
}

void PathTracingThreadPool::leaderRoutine(TaskQueue& tasks, TaskDivider& divider)
{
    Task task;
    while (divider.getNextTask(task))
    {
        tasks.enqueue(task);
    }
    tasks.stop(); // Tell threads not to block if queue is empty, but to quit
}

Color castShadowRays(const ObjectSet& objSet, const Direction& normal,
        const Point& hit, const Color& kd)
{
    Color color {0, 0, 0};
    for (const PointLight& light : objSet.pointLights)
    {
        const Direction d = light.position() - hit;
        const Real d2 = dot(d, d);
        const Real distance = std::sqrt(d2);
        const Direction dN = d / distance; // normalized d
        
        const Direction epsilon = dN * 0.0001;

        const Ray shadowRay {hit + epsilon, dN};
        auto isThereNearerObject = [&](Real distance) -> bool
        {
            for (const Shape& obj : objSet.objects)
            {
                const auto its = obj.intersect(shadowRay);
                if (Ray::isHit(its) && its < distance)
                    return true;
            }
            return false;
        };

        if (isThereNearerObject(distance))
            continue;

        const Color emission = light.color() / d2;
        const Real term = std::abs(dot(normal, dN));
        color = color + (emission * kd / numbers::pi) * term;
    }
    return color;
}

auto findIntersection(const ObjectSet& objSet, const Ray& ray)
{
    Real t = Ray::nohit;
    const Shape *hitObj = nullptr;
    for (const Shape& obj : objSet.objects)
    {
        const auto its = obj.intersect(ray);
        if (Ray::isHit(its) && (!Ray::isHit(t) || its < t))
        {
            t = its;
            hitObj = &obj;
        } 
    }
    return std::pair{t, hitObj};
}

Direction rotatedDirection(const Direction& normal, const Point& hit,
        const Real lat, const Real az)
{
    //
    const Direction ortogonal1 = (std::abs(normal[0]) < 0.1)
        ? normalize(Direction{0, normal[2], -normal[1]})
        : normalize(Direction{normal[1], -normal[0], 0});

    // const Direction ref = ((std::abs(std::abs(normal[1]) - 1) < 0.5) ?
    //         Direction{0, 0, 1} : Direction{0, 1, 0});

    // const Direction ortogonal1 = normalize(cross(normal, ref));
    const Direction ortogonal2 = cross(ortogonal1, normal);

    const Base local {normal, ortogonal2, ortogonal1, hit};
    Transformation rotation {};
    rotation.rotateY(-lat).rotateX(az).revertBase(local);

    return normalize(rotation * Direction{1, 0, 0});
}

#if 0
Color trace(const ObjectSet& objSet, const Ray& ray, Randomizer&)
{
    auto [t, hitObj] = findIntersection(objSet, ray);
    return (Ray::isHit(t)) ? hitObj->color() : Color{0, 0, 0};  
}
#elif 0
Color trace(const ObjectSet& objSet, const Ray& ray, Randomizer&)
{
    auto [t, hitObj] = findIntersection(objSet, ray);
    if (!Ray::isHit(t))
        return {0, 0, 0};

    Point hit = ray.hitPoint(t);
    auto [from, normal] = hitObj->normal(ray.d, hit);
    return castShadowRays(objSet, normal, hit, *hitObj);  
}
#elif 1
Color trace(const ObjectSet& objSet, const Ray& ray, Randomizer& random,
        const Integer bounces = 1000)
{
    if (bounces == 0)
        return Color{};

    auto [t, hitObj] = findIntersection(objSet, ray);

    if (!Ray::isHit(t))
        return Color{};

    const Material& material = hitObj->material();

    if (material.emits)
        return material.kd;

    Point hit = ray.hitPoint(t);
    auto [from, normal] = hitObj->normal(ray.d, hit);

    auto evalDiffuse = [&]()
    {
        // Direct light
        const Color directLight = castShadowRays(objSet, normal, hit, material.kd);

        // Generate random ray
        const Real lat = std ::acos(std::sqrt(1-random()));
        const Real az = 2 * numbers::pi * random();

        const Direction d = rotatedDirection(normal, hit, lat, az);
        const Direction epsilon = d * 0.0001;
        Ray secondaryRay {hit + epsilon, d};

        // Indirect light
        const Color indirectLight = trace(objSet, secondaryRay, random, bounces - 1);

        // Render eq
        return indirectLight * material.kd + directLight;
    };

    auto evalReflexion = [&]()
    {
        Direction d = ray.d - 2 * normal * dot(ray.d, normal);
        const Direction epsilon = d * 0.0001;
        Ray reflectedRay {hit + epsilon, d};
        return material.ks * trace(objSet, reflectedRay, random, bounces - 1);
    };

    auto evalRefraction = [&]()
    {
        const Direction n = -1 * normal;
        const Real hIndex = from == Side::in ? material.hIndex : 1 / material.hIndex;
        const Real latOut = std::acos(dot(ray.d, n));
        const Real latIn = std::asin(std::sin(latOut) * hIndex);

        const Direction rotRef = normalize(cross(n, ray.d));
        const Direction ortogonal2 = cross(rotRef, n);

        const Base local {n, ortogonal2, rotRef, hit};
        Transformation rotation {};
        rotation.rotateZ(latIn).revertBase(local);

        const Direction d = normalize(rotation * Direction{1, 0, 0});
        const Direction epsilon = d * 0.0001;
        Ray refractedRay {hit + epsilon, d};

        return material.kt * trace(objSet, refractedRay, random, bounces - 1);
    };

    // Russian Roulette
    Real pd = material.kd.p(), ps = material.ks.p(), pt = material.kt.p();
    Real sum = pd + ps + pt;
    if (sum > 1)
        { pd /= sum;  ps /= sum;  pt /= sum; }
    Real x = random();
    if (x <= pd)                return evalDiffuse() / pd;
    else if (x - pd <= ps)      return evalReflexion() / ps;
    else if (x - pd - ps <= pt) return evalRefraction() / pt;
    else return Color{};
}
#endif

void PathTracingThreadPool::workerRoutine(TaskQueue& tasks,
        const Camera& camera, Image& img, const ObjectSet& objects, Index ppp,
        TextProgressBar& progressBar)
{
    // Each thread has its own unique camera, to avoid critical section
    // at generating random numbers.
    Camera cam {camera};
    Randomizer random {0, 1};
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
                meanColor = meanColor + trace(objects, ray, random);
            }
            // Thread-safe operation: a pixel is not assigned to two different threads 
            img(i, j) = RGBPixel (meanColor / ppp);
        }
        progressBar.incrementProgress();
    }
}

void PathTracingThreadPool::render(const Camera& cam, Image& img,
        const ObjectSet& objects, Index ppp)
{
    float totalSize = taskDivider.width * taskDivider.height;
    float regionSize = taskDivider.regionWidth * taskDivider.regionHeight;
    TextProgressBar progressBar {regionSize / totalSize};

    leader = std::thread(leaderRoutine, 
            std::ref(tasks), std::ref(taskDivider));
    for (auto& worker : threadPool)
    {
        worker = std::thread(workerRoutine, std::ref(tasks),
                std::ref(cam), std::ref(img), std::ref(objects), ppp,
                std::ref(progressBar));
    }

    progressBar.launch();

    leader.join();
    for (auto& worker : threadPool)
        worker.join();

    progressBar.stop();
    progressBar.join();
}
