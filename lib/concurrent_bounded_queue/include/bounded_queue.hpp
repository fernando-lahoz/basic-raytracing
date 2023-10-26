#pragma once

template <class T>
class BoundedQueue {
public:
    using Size = size_t;

    BoundedQueue(const Size n);
    BoundedQueue(const BoundedQueue& queue);
    BoundedQueue(BoundedQueue&& queue);
    ~BoundedQueue();

    void clear();

    void enqueue(const T& element);

    void dequeue();

    T first();

    Size length();

    Size size();

    bool empty();

private:
    Size maxSize;
    Size numElem;
    T *data;
    Size head, next;
};

#include "src/bounded_queue.ipp"
