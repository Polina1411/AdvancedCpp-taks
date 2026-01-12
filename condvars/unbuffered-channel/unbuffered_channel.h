#pragma once

#include <utility>
#include <optional>
#include <mutex>
#include <vector>
#include <condition_variable>

template <class T>
class UnbufferedChannel {
private:
    bool closed_;
    std::condition_variable can_push_;
    std::condition_variable can_pop_;
    std::optional<T> buffer_;
    bool ready_;
    std::mutex mutex_;

public:
    UnbufferedChannel() : closed_(false), ready_(false) {
    }
    void Send(const T& value) {
        auto unique_lock = std::unique_lock<std::mutex>{mutex_};
        can_push_.wait(unique_lock,
                       [this]() { return (!buffer_.has_value() && ready_) || closed_; });
        if (closed_) {
            throw std::runtime_error("закрыт канальчик, не пройти");
        }
        buffer_ = value;
        ready_ = false;
        unique_lock.unlock();
        can_pop_.notify_one();
    }

    std::optional<T> Recv() {
        auto unique_lock = std::unique_lock<std::mutex>{mutex_};
        ready_ = true;
        can_push_.notify_one();
        can_pop_.wait(unique_lock, [this]() { return buffer_.has_value() || closed_; });
        if (!buffer_.has_value()) {
            return std::nullopt;
        }
        auto val = std::move(*buffer_);
        buffer_.reset();
        unique_lock.unlock();
        return val;
    }

    void Close() {
        auto unique_lock = std::unique_lock<std::mutex>{mutex_};
        closed_ = true;
        can_push_.notify_all();
        can_pop_.notify_all();
    }
};
