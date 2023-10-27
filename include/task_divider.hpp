#pragma once

#include "numbers.hpp"
#include "image.hpp"

#include "queue/concurrent_bounded_queue.hpp"

struct Task
{
    struct Indices { Index i, j; };
    Indices start;
    Indices end;
};

using TaskQueue = ConcurrentBoundedQueue<Task>;

class TaskDivider
{
protected:
    Index i, j;
    const Index width, height;
public:
    TaskDivider(Dimensions dim)
        : i{0}, j{0}, width{dim.width}, height{dim.height} {}

    virtual bool getNextTask(Task& task) = 0;

    void reset() { i = 0; j = 0; }
};

class PixelDivider : public TaskDivider
{
public:
    PixelDivider(Dimensions dim)
        : TaskDivider{dim} {}

    virtual bool getNextTask(Task& task);
};

class RowDivider : public TaskDivider
{
public:
    RowDivider(Dimensions dim)
        : TaskDivider{dim} {}

    virtual bool getNextTask(Task& task);
};

class ColumnDivider : public TaskDivider
{
public:
    ColumnDivider(Dimensions dim)
        : TaskDivider{dim} {}

    virtual bool getNextTask(Task& task);
};

class RegionDivider : public TaskDivider
{
private:
    Index regionWidth, regionHeight;
public:
    RegionDivider(Dimensions dim, Dimensions regionDim)
        : TaskDivider{dim},
          regionWidth{regionDim.width}, regionHeight{regionDim.height} {}

    virtual bool getNextTask(Task& task);
};
