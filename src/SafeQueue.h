//
// Created by wellsjiang on 2020/11/14.
//

#ifndef SIMPLE_SERVER_SAFEQUEUE_H
#define SIMPLE_SERVER_SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

// A thread safe queue.
template<class T>
class SafeQueue {
public:
    SafeQueue()
            : q(), m(), c() {}

    ~SafeQueue() = default;

    // Add an element to the queue.
    void enqueue(T t) {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is available.
    T dequeue() {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty()) {
            // release lock as long as the wait and requaire it afterwards.
            c.wait(lock);
        }
        if (q.empty()) {
            return nullptr;
        }
        T val = q.front();
        q.pop();
        return val;
    }

    void destroy() {
        c.notify_all();
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};

#endif //SIMPLE_SERVER_SAFEQUEUE_H
