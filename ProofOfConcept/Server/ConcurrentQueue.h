#ifndef __CONCURRENT_QUEUE_H__
#define __CONCURRENT_QUEUE_H__

#include <queue>
#include <mutex>

template <class T, class Container = std::deque<T> >
class ConcurrentQueue
{
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;

public:

    void push(const_reference t) {
        std::lock_guard<std::mutex> q_lk(mtx);
        queue.push(t);
    }

    reference& pop() {
        std::lock_guard<std::mutex> q_lk(mtx);
        reference& t = queue.front();
        queue.pop();
        return t;
    }

    const_reference& pop() const {
        std::lock_guard<std::mutex> q_lk(mtx);
        const_reference& t = queue.front();
        queue.pop();
        return t;
    }

    bool empty() {
        std::lock_guard<std::mutex> q_lk(mtx);
        return queue.empty();
    }

private:
    std::queue<T, Container> queue;
    std::mutex mtx;
};

#endif