#pragma once

#include <utility>
#include <mutex>
#include <condition_variable>
#include <optional>


template <class T>
class BufferedChannel {
private:
    size_t size_;
    bool closed_;
    std::condition_variable can_push_;
    std::condition_variable can_pop_;
    std::optional<T> buffer_;
    std::mutex mutex_;

public:
    explicit BufferedChannel(size_t size) : size_(size), closed_(false) {
    }

    void Send(const T& value) {
        auto unique_lock = std::unique_lock<std::mutex>{mutex_};
        can_push_.wait(unique_lock, [this]() { return buffer_.size() < size_ || closed_; });
        if (closed_) {
            throw std::runtime_error("закрыт канальчик, не пройти");
        }
        buffer_.push_back(value);
        can_pop_.notify_one();
    }

    std::optional<T> Recv() {
        auto unique_lock = std::unique_lock<std::mutex>{mutex_};
        can_pop_.wait(unique_lock, [this]() { return !buffer_.empty() || closed_; });
        if (buffer_.empty()) {
            return std::nullopt;
        }
        auto val = buffer_[0];
        buffer_.erase(buffer_.begin());
        can_push_.notify_one();
        return val;
    }

    void Close() {
        auto unique_lock = std::unique_lock<std::mutex>{mutex_};
        closed_ = true;
        can_push_.notify_all();
        can_pop_.notify_all();
    }
};
