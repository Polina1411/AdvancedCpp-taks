#pragma once
#include <mutex>
#include <condition_variable>


class RWLock {
public:
    template <class Func>
    void Read(Func func) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]->bool { return !writing_; });
        ++blocked_readers_;
        lock.unlock();
        func();
        {
            std::lock_guard<std::mutex> lock2(mutex_);
            --blocked_readers_;
            if (blocked_readers_ == 0) {
                cv_.notify_all();
            }
        }
    }

    template <class Func>
    void Write(Func func) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]->bool { return blocked_readers_ == 0 && !writing_; });
        writing_ = true;
        lock.unlock();
        func();
        {
            std::lock_guard<std::mutex> lock2(mutex_);
            writing_ = false;
        }
        cv_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int blocked_readers_ = 0;
    bool writing_ = false;
};