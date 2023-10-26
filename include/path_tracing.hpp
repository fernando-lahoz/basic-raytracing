#pragma once

#include "geometry.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"

#include "concurrent_bounded_queue.hpp"

#include <thread>
#include <vector>

struct Task
{
    struct Indices { Index i, j; };
    Indices start;
    Indices end;
};

using TaskQueue = ConcurrentBoundedQueue<Task>;

class TaskDivisionMachine
{
protected:
    Index i, j;
    const Index width, height;
public:
    TaskDivisionMachine(Dimensions dim)
        : i{0}, j{0}, width{dim.width}, height{dim.height} {}

    virtual bool getNextTask(Task& task) = 0;

    void reset() { i = 0; j = 0; }
};

class PixelDivider : public TaskDivisionMachine
{
public:
    PixelDivider(Dimensions dim)
        : TaskDivisionMachine{dim} {}

    virtual bool getNextTask(Task& task);
};

class PathTracingThreadPool
{
private:
    std::vector<std::thread> threadPool;
    std::thread leader;
    TaskQueue tasks;
    std::unique_ptr<TaskDivisionMachine> taskDivider; // Has ownership

public:
    static constexpr Index totalConcurrency = 0;

    PathTracingThreadPool(const Index numWorkers, const Index queueSize,
            std::unique_ptr<TaskDivisionMachine>&& divider);

    void render(const Camera& cam, Image& img,
            const ObjectSet& objects, Index ppp);
};

void leaderRoutine(TaskQueue& tasks, TaskDivisionMachine& divider);

void workerRoutine(TaskQueue& tasks, const Camera& cam, Image& img,
        const ObjectSet& objects, Index ppp);
