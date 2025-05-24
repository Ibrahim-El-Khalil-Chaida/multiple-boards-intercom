#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace mbi {

template<typename T>
class MessageQueue {
public:
    void push(const T& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(msg);
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]{ return !queue_.empty(); });
        T m = queue_.front();
        queue_.pop();
        return m;
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}