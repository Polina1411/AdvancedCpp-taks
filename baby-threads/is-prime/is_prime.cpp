#include "is_prime.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <atomic>

bool IsPrime(uint64_t x) {
    if (x <= 1) {
        return false;
    }
    uint64_t root = sqrt(x);
    auto bound = std::min(root + 6, x);
    int tread_num = 8;
    std::atomic<bool> is_prime{true};
    std::vector<std::thread> threads;
    for (int i = 0; i < tread_num; ++i) {
        threads.emplace_back([&, i, tread_num] {
            for (auto j = 2ull + i; is_prime && j < bound; j += tread_num) {
                if (x % j == 0) {
                    is_prime = false;
                    return;
                }
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return is_prime;
}
