#pragma once
#include <vector>
struct Sum {
    template <typename T>
    T operator()(T& first, const T& second) {
        return first += second;
    }
};

struct Prod {
    template <typename T>
    T operator()(T& first, const T& second) {
        return first *= second;
    }
};

struct Concat {
    template <typename T>
    std::vector<T> operator()(std::vector<T>& first, const std::vector<T>& second) {
        first.reserve(first.size() + second.size());
        for (const auto& i : second) {
            first.push_back(i);
        }
        return first;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    for (auto it = first; it != last; ++it) {
        func(init, *it);
    }
    return init;
}
struct Length {
private:
    int* len_;

public:
    Length(int* len) : len_(len) {
        *len = 0;
    }
    template <typename T>
    void operator()(T&, T&) {
        ++*len_;
    }
};