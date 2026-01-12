#pragma once
#include <algorithm>
#include <memory>
#include <optional>
#include <variant>
#include <exception>
#include <utility>
#include <stdexcept>
#include <cstring>
template <class T>
class Try {
private:
    std::optional<std::variant<T, std::exception_ptr>> exep_val_;

public:
    Try() = default;
    Try(T error) : exep_val_(std::move(error)) {
    }
    template <typename Exeption>
    Try(const Exeption& exeption) : exep_val_(std::make_exception_ptr((exeption))) {
    }
    Try(std::exception_ptr exeption) : exep_val_(exeption) {
    }
    const T& Value() const {
        if (!exep_val_.has_value()) {
            throw std::runtime_error("Object is empty");
        }
        const auto& exep_val = exep_val_.value();
        size_t ind = exep_val.index();
        switch (ind) {
            case 0:
                return std::get<0>(exep_val);
            case 1:
                std::rethrow_exception(std::get<1>(exep_val));
            default:
                throw "meow";
        }
    }
    void Throw() const {
        if (!IsFailed()) {
            throw std::runtime_error("No exception");
        }
        std::rethrow_exception(std::get<1>(exep_val_.value()));
    }
    bool IsFailed() const {
        return exep_val_.has_value() && exep_val_.value().index() == 1;
    }
};
struct Empty {};
template <>
class Try<void> : private Try<Empty> {
public:
    template <class Function, class... Args>
    friend auto TryRun(Function func, Args... args);
    Try() = default;
    Try(std::exception_ptr exeption) : Try<Empty>(exeption) {
    }
    template <typename Exeption>
    Try(const Exeption& exeption) : Try<Empty>(exeption) {
    }
    void Throw() const {
        Try<Empty>::Throw();
    }
    bool IsFailed() const {
        return Try<Empty>::IsFailed();
    }
};
template <class Function, class... Args>
auto TryRun(Function func, Args... args)
    -> std::enable_if_t<!std::is_void_v<decltype(func(args...))>, Try<decltype(func(args...))>> {
    using ReturnType = decltype(func(args...));
    try {
        auto res = func(args...);
        return Try<ReturnType>(std::move(res));
    } catch (int e) {
        return Try<ReturnType>(std::runtime_error(std::strerror(e)));
    } catch (const char* e) {
        return Try<ReturnType>(std::runtime_error(e));
    } catch (const std::exception& e) {
        return Try<ReturnType>(std::current_exception());
    } catch (...) {
        return Try<ReturnType>(std::runtime_error("Unknown exception"));
    }
    return Try<ReturnType>{};
}
template <class Function, class... Args>
auto TryRun(Function func, Args... args)
    -> std::enable_if_t<std::is_void_v<decltype(func(args...))>, Try<decltype(func(args...))>> {
    try {
        func(args...);
        return Try<void>();
    } catch (int e) {
        return Try<void>(std::runtime_error(std::strerror(e)));
    } catch (const char* e) {
        return Try<void>(std::runtime_error(e));
    } catch (const std::exception& e) {
        return Try<void>(std::current_exception());
    } catch (...) {
        return Try<void>(std::runtime_error("Unknown exception"));
    }
}