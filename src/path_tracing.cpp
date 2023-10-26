#include "path_tracing.hpp"

bool PixelDivider::getNextTask(Task& task)
{
    if (i == height - 1 && j == width)
        return false;

    if (j == width)
    {
        j = 0;
        i++;
    }
    
    task.start = {i, j};
    task.end = {i + 1, j + 1};

    j++;

    return true;
}

PathTracingThreadPool::
PathTracingThreadPool(const Index numWorkers, const Index queueSize,
        std::unique_ptr<TaskDivisionMachine>&& divider)

    : tasks{queueSize}, taskDivider{std::move(divider)}
{
    auto hw = std::thread::hardware_concurrency();
    const Index nThreads = numWorkers <= 0
            ? numbers::max(hw, (decltype(hw))1)
            : numWorkers;

    threadPool.resize(nThreads);
}

void leaderRoutine(TaskQueue& tasks,
        TaskDivisionMachine& divider)
{
    Task task;
    while (divider.getNextTask(task))
    {
        tasks.enqueue(task);
    }
    tasks.stop(); //WARNING: Hasta que se acabe, no parar de golpe
}

void workerRoutine(TaskQueue& tasks, const Camera& camera,
        Image& img, const ObjectSet& objects, Index ppp)
{
    Camera cam {camera};
    Task task {};
    while (tasks.dequeue(task))
    {
        for (Index i : std::views::iota(task.start.i, task.end.i))
        for (Index j : std::views::iota(task.start.j, task.end.j))
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
            for ([[maybe_unused]] Index k : std::views::iota(Index{0}, ppp))
            {
                Ray ray = cam.randomRay(i, j);
                Emission color = trace(ray);
                meanColor.r += color.r;
                meanColor.g += color.g;
                meanColor.b += color.b;
            }
            // Thread-safe operation: a pixel is not assigned to two different threads 
            img(i, j) = RGBPixel {
                meanColor.r / ppp,
                meanColor.g / ppp,
                meanColor.b / ppp
            };
        }
    }
}

void PathTracingThreadPool::render(const Camera& cam, Image& img,
        const ObjectSet& objects, Index ppp)
{
    leader = std::thread(leaderRoutine, 
            std::ref(tasks), std::ref(*taskDivider));
    for (auto& worker : threadPool)
    {
        worker = std::thread(workerRoutine, std::ref(tasks),
                std::ref(cam), std::ref(img), std::ref(objects), ppp);
    }
        

    leader.join();
    for (auto& worker : threadPool)
        worker.join();
}
