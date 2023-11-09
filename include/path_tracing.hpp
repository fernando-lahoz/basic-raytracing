#pragma once

#include "geometry.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"
#include "light.hpp"
#include "object_set.hpp"

#include "queue/concurrent_bounded_queue.hpp"
#include "progress_bar.hpp"

#include <thread>
#include <vector>

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

class PathTracingThreadPool
{
private:
    std::vector<std::thread> threadPool;
    std::thread leader;
    TaskQueue tasks;
    TaskDivider taskDivider;
public:
    static constexpr Index totalConcurrency = 0;

    PathTracingThreadPool(const Index numWorkers, const Index queueSize,
            const TaskDivider& divider);

    static void leaderRoutine(TaskQueue& tasks, TaskDivider& divider);

    static void workerRoutine(TaskQueue& tasks, const Camera& camera,
            Image& img, const ObjectSet& objects, Index ppp,
            ProgressBar& progressBar);

    void render(const Camera& cam, Image& img,
            const ObjectSet& objects, Index ppp);

    inline Index numThreads() {return threadPool.size(); }
};

