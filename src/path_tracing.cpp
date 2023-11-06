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
    task.end = {numbers::min(i + regionHeight, height),
                numbers::min(j + regionWidth, width)};

    j = numbers::min(j + regionWidth, width);

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

#if 0
Color trace(const ObjectSet& objSet, const Ray& ray)
{
    Real t = Ray::nohit;
    const Shape* hitObj = nullptr;
    for (const Shape& obj : objSet.objects)
    {
        const auto its = obj.intersect(ray);
        if (Ray::isHit(its) && (!Ray::isHit(t) || its < t))
        {
            t = its;
            hitObj = &obj;
        } 
    }

    return (hitObj != nullptr) ? hitObj->color() : Color{0, 0, 0};  
}
#elif 1
Color trace(const ObjectSet& objSet, const Ray& ray)
{
    Real t = Ray::nohit;
    const Shape* hitObj = nullptr;
    for (const Shape& obj : objSet.objects)
    {
        const auto its = obj.intersect(ray);
        if (Ray::isHit(its) && (!Ray::isHit(t) || its < t))
        {
            t = its;
            hitObj = &obj;
        }
    }
    if (!Ray::isHit(t))
        return {0, 0, 0};

    Point hit = ray.hitPoint(t);
    Direction normal = hitObj->normal(ray.d, hit);
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
        color = color + (emission * hitObj->color() * (1 / numbers::pi) * term);
    }

    return color;  
}
#endif

void PathTracingThreadPool::workerRoutine(TaskQueue& tasks,
        const Camera& camera, Image& img, const ObjectSet& objects, Index ppp)
{
    // Each thread has its own unique camera, to avoid critical section
    // at generating random numbers.
    Camera cam {camera};
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
                meanColor = meanColor + trace(objects, ray);
            }
            // Thread-safe operation: a pixel is not assigned to two different threads 
            img(i, j) = RGBPixel (meanColor / ppp);
        }
    }
}

void PathTracingThreadPool::render(const Camera& cam, Image& img,
        const ObjectSet& objects, Index ppp)
{
    leader = std::thread(leaderRoutine, 
            std::ref(tasks), std::ref(taskDivider));
    for (auto& worker : threadPool)
    {
        worker = std::thread(workerRoutine, std::ref(tasks),
                std::ref(cam), std::ref(img), std::ref(objects), ppp);
    }

    leader.join();
    for (auto& worker : threadPool)
        worker.join();
}
