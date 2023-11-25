#include "path_tracing.hpp"

using namespace PathTracing;

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

PathTracing::Renderer::
Renderer(const Index numWorkers, const Index queueSize,
        const TaskDivider& divider, Strategy strategy)
    : tasks{queueSize}, taskDivider{divider}
{
    trace = [&]()
    {
        switch (strategy)
        {
        case Strategy::projection: return traceProjection;
        case Strategy::direct:     return traceDirectLight;
        case Strategy::recursive:  return traceIndirectLightRecursive;
        case Strategy::iterative:  return traceIndirectLightRecursive;
        default:                   return traceIndirectLightRecursive;
        }
    }();

    auto hw = std::thread::hardware_concurrency();
    const Index nThreads = numWorkers <= 0
            ? numbers::max(hw, (decltype(hw))1)
            : numWorkers;

    threadPool.resize(nThreads);
}

Index PathTracing::Renderer::
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

Color PathTracing::
traceProjection(const ObjectSet& objSet, const Ray& ray, Randomizer&)
{
    auto [t, hitObj] = findIntersection(objSet, ray);
    return (Ray::isHit(t)) ? hitObj->material().kd() : Color{0, 0, 0};  
}

Color PathTracing::
traceDirectLight(const ObjectSet& objSet, const Ray& ray, Randomizer&)
{
    auto [t, hitObj] = findIntersection(objSet, ray);
    if (!Ray::isHit(t))
        return {0, 0, 0};

    Point hit = ray.hitPoint(t);
    auto [from, normal] = hitObj->shape().normal(ray.d, hit);
    return castShadowRays(objSet, normal, hit, hitObj->material().kd());  
}

Color traceIndirectLightRecursiveLimited(const ObjectSet& objSet, const Ray& ray,
        Randomizer& random, const Integer bounces)
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

    const Color indirectLight = traceIndirectLightRecursiveLimited(objSet, secondaryRay, random, bounces - 1);

    if (k == Material::Component::kd)
    {
        const Color directLight = castShadowRays(objSet, normal.normal, hit, material.kd());
        return indirectLight * color + directLight;
    }
    
    return indirectLight * color;
}

Color PathTracing::
traceIndirectLightRecursive(const ObjectSet& objSet, const Ray& ray,
        Randomizer& random)
{
    return traceIndirectLightRecursiveLimited(objSet, ray, random, -1); /*-1 for unlimited*/
}

void workerRoutine(TaskQueue& tasks, Real increment, const Camera& camera,
        Image& img, const ObjectSet& objects, Index ppp,
        TextProgressBar& progressBar, TraceFunction trace)
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
        progressBar.incrementProgress(increment);
    }
}

void PathTracing::Renderer::
render(const Camera& cam, Image& img,
        const ObjectSet& objects, Index ppp)
{
    std::cout << "Algorithm: path tracing\n";
    std::cout << "Worker pool size: " << numThreads() << "\n\n";

    const Real totalSize = taskDivider.width * taskDivider.height;
    const Real regionSize = taskDivider.regionWidth * taskDivider.regionHeight;
    const Real increment = regionSize / totalSize;

    TextProgressBar progressBar;

    leader = std::thread(leaderRoutine, 
            std::ref(tasks), std::ref(taskDivider));
    for (auto& worker : threadPool)
    {
        worker = std::thread(workerRoutine, std::ref(tasks), increment,
                std::ref(cam), std::ref(img), std::ref(objects), ppp,
                std::ref(progressBar), trace);
    }

    std::cout << "Rendering...\n";
    progressBar.launch();

    leader.join();
    for (auto& worker : threadPool)
        worker.join();

    progressBar.stop();
    progressBar.join();

    img.updateLuminance();
}
