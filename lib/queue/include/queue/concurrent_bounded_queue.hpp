#pragma once

#include <mutex>
#include <condition_variable>

#include "queue/bounded_queue.hpp"

template <typename Ty>
class ConcurrentBoundedQueue
{
private:
    std::condition_variable waitEnqueue, waitDequeue;
    std::mutex mtx;
    BoundedQueue<Ty> queue;
    bool stopped;

public:
    using Size = size_t;

    ConcurrentBoundedQueue(Size n);

    ConcurrentBoundedQueue(const ConcurrentBoundedQueue& other);

    ConcurrentBoundedQueue(ConcurrentBoundedQueue&& other);

    void enqueue(const Ty& element);

    bool dequeue(Ty& element);

    // Stops the use of this queue and awakes every blocked thread
    void stop();
};

#include "src/concurrent_bounded_queue.ipp"
