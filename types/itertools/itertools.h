#pragma once
#include <utility>
#include <tuple>
template <class Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
    }

    Iterator begin() const {  // NOLINT
        return begin_;
    }

    Iterator end() const {  // NOLINT
        return end_;
    }

private:
    Iterator begin_, end_;
};
template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
class RangeIt {
private:
    T cur_ = 0;
    T end_ = 0;
    T step_ = 1;

public:
    RangeIt(const T cur = 0, const T end = 0, const T step = 1)
        : cur_(cur), end_(end), step_(step) {
    }
    RangeIt& operator++() {
        if (cur_ < end_) {
            cur_ += step_;
            if (cur_ >= end_) {
                cur_ = end_;
            }
        }
        return *this;
    }
    T operator*() const {
        return cur_;
    }
    bool operator==(const RangeIt<T>& other) const {
        return std::tie(cur_, end_, step_) == std::tie(other.cur_, other.end_, other.step_);
    }
    bool operator!=(const RangeIt<T>& other) const {
        return !(*this == other);
    }
};
template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
IteratorRange<RangeIt<T>> Range(const T& from, const T& to, const T& step) {
    auto start_it = RangeIt<T>(from, to, step);
    auto end_it = RangeIt<T>(to, to, step);
    return IteratorRange<RangeIt<T>>(start_it, end_it);
}

template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
IteratorRange<RangeIt<T>> Range(const T& from, const T& to) {
    return Range(from, to, static_cast<T>(1));
}

template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
IteratorRange<RangeIt<T>> Range(const T& to) {
    return Range(static_cast<T>(0), to);
}

template <typename FirstPos, typename SecondPos>
class ZipIt {
private:
    FirstPos first_begin_;
    SecondPos second_begin_;
    FirstPos first_end_;
    SecondPos second_end_;

public:
    ZipIt(FirstPos first_begin, SecondPos second_begin, FirstPos first_end, SecondPos second_end)
        : first_begin_(first_begin),
          second_begin_(second_begin),
          first_end_(first_end),
          second_end_(second_end) {
    }
    ZipIt& operator++() {
        ++first_begin_;
        ++second_begin_;
        if (first_begin_ == first_end_ || second_begin_ == second_end_) {
            first_begin_ = first_end_;
            second_begin_ = second_end_;
        }
        return *this;
    }
    auto operator*() const {
        return std::make_pair(*first_begin_, *second_begin_);
    }
    bool operator==(const ZipIt<FirstPos, SecondPos>& other) const {
        return std::tie(first_begin_, second_begin_, first_end_, second_end_) ==
               std::tie(other.first_begin_, other.second_begin_, other.first_end_,
                        other.second_end_);
    }
    bool operator!=(const ZipIt& other) const {
        return !(*this == other);
    }
};
template <typename FirstContainer, typename SecondContainer>
auto Zip(const FirstContainer& first, const SecondContainer& second) {
    auto start = ZipIt(first.begin(), second.begin(), first.end(), second.end());
    auto finish = ZipIt(first.end(), second.end(), first.end(), second.end());
    return IteratorRange(start, finish);
}
template <typename Iterator>
class GroupIt {
private:
    Iterator it_;
    Iterator finish_;

public:
    GroupIt(Iterator it, Iterator finish) : it_(it), finish_(finish) {
    }
    GroupIt& operator++() {
        const auto& old_value = *it_;
        while (it_ != finish_ && *it_ == old_value) {
            ++it_;
        }
        return *this;
    }
    auto operator*() {
        auto st = it_;
        auto en = it_;
        while (en != finish_ && *st == *en) {
            ++en;
        }
        return IteratorRange(st, en);
    }
    bool operator==(const GroupIt<Iterator>& other) const {
        return std::tie(it_, finish_) == std::tie(other.it_, other.finish_);
    }
    bool operator!=(const GroupIt<Iterator>& other) const {
        return !(*this == other);
    }
};
template <typename Container>
auto Group(const Container& it) {
    auto start = GroupIt(it.begin(), it.end());
    auto end = GroupIt(it.end(), it.end());
    return IteratorRange(start, end);
}