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
        const Point& hit, const Shape& hitObj)
{
    Color color {0, 0, 0};
    for (const PointLight& light : objSet.pointLights)
    {
        const Direction d = light.position() - hit;
        const Real d2 = dot(d, d);
        const Direction dN = d / std::sqrt(d2); // normalized d
        
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

        if (isThereNearerObject(norm(d)))
            continue;

        const Color emission = light.color() / d2;
        const Real term = std::abs(dot(normal, dN));
        color = color + (emission * hitObj.color() / numbers::pi) * term;
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
    Direction normal = hitObj->normal(ray.d, hit);
    return castShadowRays(objSet, normal, hit, *hitObj);  
}
#elif 1
Color trace(const ObjectSet& objSet, const Ray& ray, Randomizer& random,
        const Index bounces = 20)
{
    if (bounces == 0)
        return {0, 0, 0};

    auto [t, hitObj] = findIntersection(objSet, ray);

    if (!Ray::isHit(t))
        return {0, 0, 0};

    if (hitObj->isAreaLight())
        return hitObj->color();

    Point hit = ray.hitPoint(t);
    Direction normal = hitObj->normal(ray.d, hit);
    Color directLight = castShadowRays(objSet, normal, hit, *hitObj);

    // Generate random ray
    const Real randomLatitud = std::acos(std::sqrt(1 - random()));
    const Real randomAzimuth = 2 * numbers::pi * random();

    // ESTE MÉTODO NO GENERA VECTORES ORTOGONALES A LA NORMAL :(
    // const Direction ortogonal1 = normal[0] == 0.0
    //     ? Direction{normal[0], normal[2], -normal[1]}
    //     : Direction{normal[1], -normal[0], normal[2]};

    const Direction ortogonal1 = normalize(cross(normal,
        ((normal[0] == 1) | (normal[0] == -1) ? Direction{0, 1, 0} : Direction{1, 0, 0})));
    const Direction ortogonal2 = cross(ortogonal1, normal);

    const Base local {normal, ortogonal1, ortogonal2, hit};
    Transformation rotation {};
    rotation.rotateY(randomLatitud).rotateX(randomAzimuth).revertBase(local);

    Direction d = normalize(rotation * Direction{1, 0, 0});
    const Direction epsilon = d * 0.0001;
    Ray secondaryRay {hit + epsilon, d};

    //QUITAR--------------------------
    // if (bounces == 20)
    // {
    //     std::cout << "L: " << randomLatitud << " - A: " << randomAzimuth;
    //     std::cout << " - n: " << normal << " - d: " << d << '\n';
    // }
    //--------------------------------

    Color indirectLight = trace(objSet, secondaryRay, random, bounces - 1);
  
    return indirectLight * hitObj->color() + directLight;
}
#endif

void PathTracingThreadPool::workerRoutine(TaskQueue& tasks,
        const Camera& camera, Image& img, const ObjectSet& objects, Index ppp,
        ProgressBar& progressBar)
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
            //QUITAR--------------------------
            //if (i < 404 || i > 417 || j < 126 || j > 141)
            //    continue;
            // if (i != 410 || j != 133)
            //     continue;
            //--------------------------------

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
    ProgressBar progressBar {regionSize / totalSize};

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
