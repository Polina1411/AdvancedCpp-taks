#pragma once

#include <thread>
#include <vector>
#include <mutex>

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    const int num_threads = std::thread::hardware_concurrency();
    const size_t arr_size = last - first;
    const size_t chunk_size = arr_size / num_threads;
    const size_t last_chunk_size = arr_size % num_threads;

    std::vector<std::thread> threads;
    std::vector<T> results(num_threads);
    std::mutex mutex;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i] {
            const RandomAccessIterator start = first + i * chunk_size;
            const RandomAccessIterator end =
                start + chunk_size + (i == num_threads - 1 ? last_chunk_size : 0);

            if (start == end) {
                return;
            }

            T result = *start;
            for (RandomAccessIterator it = start + 1; it != end; ++it) {
                result = func(result, *it);
            }
            std::lock_guard<std::mutex> lock_guard(mutex);
            results[i] = result;
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    T result = initial_value;
    for (int i = 0; i < num_threads; ++i) {
        result = func(result, results[i]);
    }
    return result;
}