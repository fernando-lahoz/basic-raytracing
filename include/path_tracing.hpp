#pragma once

#include "geometry.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"
#include "light.hpp"
#include "object_set.hpp"

#include "queue/concurrent_bounded_queue.hpp"
#include "progress_bar/text_progress_bar.hpp"

#include <thread>
#include <vector>

namespace PathTracing {

struct Task
{
    struct Indices { Index i, j; };
    Indices start;
    Indices end;
};

using TaskQueue = ConcurrentBoundedQueue<Task>;

class TaskDivider
{
private:
    Index i, j;
public:
    const Index width, height;
    const Index regionWidth, regionHeight;

    TaskDivider(Dimensions imageDim, Dimensions regionDim)
        : i{0}, j{0}, width{imageDim.width}, height{imageDim.height},
          regionWidth{regionDim.width}, regionHeight{regionDim.height} {}

    bool getNextTask(Task& task);
};

Color traceProjection(const ObjectSet& objSet, const Ray& ray, Randomizer&);
Color traceDirectLight(const ObjectSet& objSet, const Ray& ray, Randomizer&);
Color traceIndirectLightRecursive(const ObjectSet& objSet, const Ray& ray, Randomizer& random);

using TraceFunction = decltype(traceProjection)*;

enum class Strategy : uint8_t
{
    projection,
    direct,
    recursive,
    iterative
};

class Renderer
{
private:
    std::vector<std::thread> threadPool;
    std::thread leader;
    TaskQueue tasks;
    TaskDivider taskDivider;
    TraceFunction trace;
public:
    static constexpr Index totalConcurrency = 0;

    Renderer(const Index numWorkers, const Index queueSize,
            const TaskDivider& divider, Strategy strategy);

    void render(const Camera& cam, Image& img, const ObjectSet& objects, Index ppp);

    Index numThreads();
};

} //namespace PathTracing
