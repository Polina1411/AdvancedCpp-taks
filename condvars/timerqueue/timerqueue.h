#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <deque>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

public:
    void Add(const T& item, TimePoint at) {
        auto guard = std::lock_guard{mutex_};
        pair_queue_.push_back({at, item});
        cv_.notify_one();
    }

    T Pop() {
        // Your code goes here
        auto guard = std::unique_lock{mutex_};
        while (true) {
            cv_.wait(guard, [this] -> bool { return !pair_queue_.empty(); });
            auto mini = 0;
            for (auto i = 0; i < pair_queue_.size(); ++i) {
                if (pair_queue_[i].first < pair_queue_[mini].first) {
                    mini = i;
                }
            }
            auto& time = pair_queue_[mini].first;
            if (time > Clock::now()) {
                cv_.wait_until(guard, time);
                continue;
            }
            T res = std::move(pair_queue_[mini].second);
            pair_queue_.erase(pair_queue_.begin() + mini);
            return res;
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<std::pair<TimePoint, T>> pair_queue_;
};
