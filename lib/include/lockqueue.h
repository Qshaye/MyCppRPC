#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 模板类不能头文件和源文件分离编写

template<typename T>
class Lockqueue
{
public:
    // 多个工作线程向队列中写日志，需要互斥保证线程安全
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        // 因为只有一个日志线程，只需要notify_one
        m_condvar.notify_one();
    }

    // 一个日志线程从队列中取出数据，只有队列不空才取数据，需要条件变量
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        // while防止虚假唤醒
        while (m_queue.empty()) {
            // 日志队列为空，线程进入wait状态(并释放锁)
            m_condvar.wait(lock);

        }
        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvar;
};