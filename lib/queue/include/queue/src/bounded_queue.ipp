#pragma once

#include "queue/bounded_queue.hpp"

#include <algorithm>

template <class T>
BoundedQueue<T>::BoundedQueue(const Size n)
{
    this->maxSize = n;
    data = new T[n];
    clear();
}

template <class T>
BoundedQueue<T>::BoundedQueue(const BoundedQueue& queue)
{
    this->maxSize = queue.maxSize;
    this->numElem = queue.numElem;
    this->head = queue.head;
    this->next = queue.next;
    this->data = new T[queue.maxSize];
    std::copy(queue.data, queue.data + queue.maxSize, this->data);
}

template <class T>
BoundedQueue<T>::BoundedQueue(BoundedQueue&& queue)
{
    this->maxSize = queue.maxSize;
    this->numElem = queue.numElem;
    this->head = queue.head;
    this->next = queue.next;
    this->data = queue.data;
    queue.data = nullptr;
}

template <class T>
BoundedQueue<T>::~BoundedQueue()
{
    delete[] data;
}

template <class T>
void BoundedQueue<T>::clear()
{
    head = 0;
    next = 0;
    numElem = 0;
}

template <class T>
void BoundedQueue<T>::enqueue(const T& element)
{
    data[next] = element;
    next = (next + 1) % maxSize;
    numElem++;
}

template <class T>
void BoundedQueue<T>::dequeue()
{
    head = (head + 1) % maxSize;
    numElem--;
}

template <class T>
T BoundedQueue<T>::first()
{
    return data[head];
}

template <class T>
BoundedQueue<T>::Size BoundedQueue<T>::length()
{
    return numElem;
}

template <class T>
BoundedQueue<T>::Size BoundedQueue<T>::size()
{
    return maxSize;
}

template <class T>
bool BoundedQueue<T>::empty()
{
    return numElem == 0;
}
