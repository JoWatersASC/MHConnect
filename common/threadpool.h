#pragma once

#include <queue>
#include <functional>
#include <memory>
#include <atomic>

#include "common.h"

namespace osf
{

class ThreadPool {
public:
    ThreadPool(size_t);
    ~ThreadPool() { stop(); }

    template<typename T>
    void add(T&&);

    void start();
    void stop();
    
    const bool empty() const {
        return m_tasks.empty();
    }

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::atomic<bool> close;
	//std::ofstream Log;

};

template<typename T>
inline void ThreadPool::add(T&& _func) {
    {
        std::unique_lock<std::mutex> lock(m_mtx);
		
		//Log << "acquired lock" << std::endl;
        m_tasks.emplace(std::forward<T>(_func));
    }

    m_cv.notify_one();
}

inline ThreadPool::ThreadPool(size_t num_threads) : close(false) {
    if(num_threads == 0)
        num_threads = 1;
	//Log.open("log.txt");
    m_workers.reserve(num_threads);
}
inline void ThreadPool::start() {
    const size_t cap = m_workers.capacity();

    for(size_t i = 0; i < cap; i++) {
        m_workers.emplace_back([this]() {
            while(!this->close) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(m_mtx);
                    m_cv.wait(lock, [this]() {
                        return (!m_tasks.empty() || close);
                    });
                    if(close && m_tasks.empty())
                        return;

                    if(!m_tasks.empty()) {
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                }
                
                if(task)
                    task();
            }
        });
    }
}
inline void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        close = true;
    }
    m_cv.notify_all();

    for(auto& worker : m_workers) {
        if(worker.joinable())
            worker.join();
    }
}

template<typename T, class container = std::deque<T>>
class tqueue { //Thread safe queue
public:
    tqueue() = default;
    tqueue(const tqueue<T, container>&) = delete;
    tqueue(tqueue&&) = default;


    ~tqueue() {
        clear();
    }

    const T& front() {
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_que.front();
    }

    void enqueue(const T item) {
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_que.push(std::move(item));
        }

        m_cv.notify_one();
    }
    void dequeue() {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_que.pop();
    }
    void clear() {
        std::unique_lock<std::mutex> lock(m_mtx);
        while(!m_que.empty())
            m_que.pop();
    }
    T pop_front() {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv.wait(lock, [this]() { return !m_que.empty(); });

        T out = std::move(m_que.front());
        m_que.pop();
        return out;
    }
    bool empty() {
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_que.empty();
    }

private:
    std::queue<T, container> m_que;
    std::mutex m_mtx;
    std::condition_variable m_cv;

};

}
