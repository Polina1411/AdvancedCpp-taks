#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};
class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }
    void Leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        if (!queue_.empty()) {
            queue_.front()->notify_one();
        }
    }
    template <class Func>
    void Enter(Func callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        std::condition_variable cv;
        queue_.push(&cv);
        cv.wait(lock, [this, &cv] -> bool { return count_ > 0 && &cv == queue_.front(); });
        callback(count_);
        queue_.pop();
        if (!queue_.empty()) {
            queue_.front()->notify_one();
        }
    }
    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::queue<std::condition_variable*> queue_;
    std::mutex mutex_;
    int count_ = 0;
};