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
            for (const Object& obj : objSet.objects)
            {
                const auto its = obj.shape().intersect(shadowRay);
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
    const Object *hitObj = nullptr;
    for (const Object& obj : objSet.objects)
    {
        const auto its = obj.shape().intersect(ray);
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
    auto [from, normal] = hitObj->normal(ray.d, hit);
    return castShadowRays(objSet, normal, hit, *hitObj);  
}
#elif 1
Color trace(const ObjectSet& objSet, const Ray& ray, Randomizer& random,
        const Integer bounces = -1 /*-1 for unlimited*/)
{
    if (bounces == 0)
        return Color{};

    const auto [t, hitObj] = findIntersection(objSet, ray);

    if (!Ray::isHit(t))
        return Color{};

    const auto& shape = hitObj->shape();
    const auto& material = hitObj->material();

    const auto [emits, emission] = material.emission();
    if (emits)
        return emission;

    const auto hit = ray.hitPoint(t);
    const auto normal = shape.normal(ray.d, hit);

    Ray secondaryRay;
    const auto [color, k] = material.eval(hit, ray, secondaryRay, normal, random);

    if (k == Material::Component::ka)
        return color;

    const Color indirectLight = trace(objSet, secondaryRay, random, bounces - 1);

    if (k != Material::Component::kd)
    {
        const Color directLight = castShadowRays(objSet, normal.normal, hit, material.kd());
        return indirectLight * color + directLight;
    }
    
    return indirectLight * color;
}
#endif

void PathTracingThreadPool::workerRoutine(TaskQueue& tasks,
        const Camera& camera, Image& img, const ObjectSet& objects, Index ppp,
        TextProgressBar& progressBar)
{
    // Each thread has its own unique camera, to avoid critical section
    // at generating random numbers.
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
