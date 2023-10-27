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
            Point hit;
            Direction normal;

            auto trace = [&](const Ray& ray) -> Emission
            {
                Real minT = Ray::nohit;
                const Shape* hitObj = nullptr;
                for (const Shape& obj : objects)
                {
                    const auto its = obj.intersect(ray, minT, hit, normal);
                    if (its != Ray::nohit)
                    {
                        minT = its;
                        hitObj = &obj;
                    } 
                }

                return (hitObj != nullptr) ? hitObj->color() : Emission{0, 0, 0};
            };

            Emission meanColor {0, 0, 0};
            for ([[maybe_unused]] Index k : numbers::range(0, ppp))
            {
                Ray ray = cam.randomRay(i, j);
                meanColor = meanColor + trace(ray);
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
