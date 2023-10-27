#pragma once

#include "geometry.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"
#include "task_divider.hpp"

#include <thread>
#include <vector>

class PathTracingThreadPool
{
private:
    std::vector<std::thread> threadPool;
    std::thread leader;
    TaskQueue tasks;
    std::unique_ptr<TaskDivider> taskDivider; // Has ownership

public:
    static constexpr Index totalConcurrency = 0;

    PathTracingThreadPool(const Index numWorkers, const Index queueSize,
            std::unique_ptr<TaskDivider>&& divider);

    void render(const Camera& cam, Image& img,
            const ObjectSet& objects, Index ppp);
};

void leaderRoutine(TaskQueue& tasks, TaskDivider& divider);

void workerRoutine(TaskQueue& tasks, const Camera& camera,
        Image& img, const ObjectSet& objects, Index ppp);
