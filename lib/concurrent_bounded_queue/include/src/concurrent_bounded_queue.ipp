#pragma once

#include "concurrent_bounded_queue.hpp"

template <typename Ty>
ConcurrentBoundedQueue<Ty>::
ConcurrentBoundedQueue(Size n)
    : queue{n}, stopped{false}
{}

template <typename Ty>
ConcurrentBoundedQueue<Ty>::
ConcurrentBoundedQueue(const ConcurrentBoundedQueue& other)
{
    std::unique_lock<std::mutex> lock(mtx);

    queue = other.queue;
    stopped = other.stopped;
}

template <typename Ty>
ConcurrentBoundedQueue<Ty>::
ConcurrentBoundedQueue(ConcurrentBoundedQueue&& other)
{
    std::unique_lock<std::mutex> lock(mtx);

    queue = std::move(other.queue);
    stopped = other.stopped;
}

template <typename Ty>
void ConcurrentBoundedQueue<Ty>::enqueue(const Ty& element)
{
    std::unique_lock<std::mutex> lock(mtx);

    while(queue.length() == queue.size() && !stopped)
    {
        waitEnqueue.wait(lock);
    }
    
    if (!stopped)
    {
        queue.enqueue(element);

        waitDequeue.notify_one();
    }
}

template <typename Ty>
bool ConcurrentBoundedQueue<Ty>::dequeue(Ty& element)
{
    std::unique_lock<std::mutex> lock(mtx);

    while(queue.empty() && !stopped)
    {
        waitDequeue.wait(lock);
    }

    if(!stopped || !queue.empty())
    {
        element = queue.first();
        queue.dequeue();

        waitEnqueue.notify_one();
        
        return true;
    }
    return false;
}

template <typename Ty>
void ConcurrentBoundedQueue<Ty>::stop()
{
    std::unique_lock<std::mutex> lock(mtx);

    stopped = true;
    waitEnqueue.notify_all();
    waitDequeue.notify_all();
}