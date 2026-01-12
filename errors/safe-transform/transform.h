#pragma once
#include <vector>

template <typename Iter, typename Pred, typename Func>

void TransformIf(Iter begin, Iter end, const Pred& p, const Func& f) {
    using T = typename Iter::value_type;
    std::vector<std::pair<Iter, T>> save_values;
    save_values.reserve(std::distance(begin, end));
    bool ex = false;
    try {
        for (Iter i = begin; i != end; ++i) {
            if (p(*i)) {
                if (!ex) {
                    try {
                        save_values.emplace_back(i, *i);
                    } catch (...) {
                        ex = true;
                    }
                }
                f(*i);
            }
        }
    } catch (...) {
        if (ex) {
            throw;
        }
        for (auto& [it, val] : save_values) {
            *it = val;
        }
        throw;
    }
}