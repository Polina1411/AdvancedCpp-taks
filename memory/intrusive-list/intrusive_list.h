#pragma once

#include <algorithm>
#include <iterator>

class ListHook {
public:
    ListHook() : right_neighbour_(nullptr), left_neighbour_(nullptr) {
    }

    bool IsLinked() const {
        return right_neighbour_ != nullptr && left_neighbour_ != nullptr;
    }

    void Unlink() {
        if (right_neighbour_ != nullptr) {
            right_neighbour_->left_neighbour_ = left_neighbour_;
        }
        if (left_neighbour_ != nullptr) {
            left_neighbour_->right_neighbour_ = right_neighbour_;
        }
        right_neighbour_ = nullptr;
        left_neighbour_ = nullptr;
    }

    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;

private:
    template <class T>
    friend class List;
    ListHook* right_neighbour_;
    ListHook* left_neighbour_;
    void LinkBefore(ListHook* other) {
        ListHook* pred = other[0].left_neighbour_;
        pred[0].right_neighbour_ = this;
        this[0].left_neighbour_ = pred;
        this[0].right_neighbour_ = other;
        other[0].left_neighbour_ = this;
    }
};

template <typename T>
class List {
public:
    class Iterator {
    public:
        Iterator(ListHook* lh) : lh_(lh) {
        }
        using IteratorTag = std::bidirectional_iterator_tag;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;
        typedef IteratorTag iterator_category;

        Iterator& operator++() {
            if (lh_) {
                lh_ = lh_[0].right_neighbour_;
            }
            return *this;
        }
        Iterator operator++(int) {
            auto result = *this;
            ++(*this);
            return result;
        }
        T& operator*() const {
            return *static_cast<T*>(lh_);
        }
        T* operator->() const {
            return static_cast<T*>(lh_);
        }
        bool operator==(const Iterator& rhs) const {
            return lh_ == rhs.lh_;
        }
        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

    private:
        ListHook* lh_;
    };

    List() {
        dummy_.left_neighbour_ = &dummy_;
        dummy_.right_neighbour_ = &dummy_;
    }
    List(const List&) = delete;
    List(List&& other) {
        dummy_.right_neighbour_ = &dummy_;
        dummy_.left_neighbour_ = &dummy_;

        ListHook* first = other.dummy_.right_neighbour_;
        ListHook* second = other.dummy_.left_neighbour_;

        first[0].left_neighbour_ = &dummy_;
        second[0].right_neighbour_ = &dummy_;

        dummy_.right_neighbour_ = first;
        dummy_.left_neighbour_ = second;

        other.dummy_.right_neighbour_ = &other.dummy_;
        other.dummy_.left_neighbour_ = &other.dummy_;
    }

    ~List() {
        while (!IsEmpty()) {
            PopFront();
        }
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        ListHook* first = other.dummy_.right_neighbour_;
        ListHook* second = other.dummy_.left_neighbour_;
        first[0].left_neighbour_ = &dummy_;
        second[0].right_neighbour_ = &dummy_;
        dummy_.right_neighbour_ = first;
        dummy_.left_neighbour_ = second;
        other.dummy_.right_neighbour_ = &other.dummy_;
        other.dummy_.left_neighbour_ = &other.dummy_;
        return *this;
    }

    bool IsEmpty() const {
        return dummy_.right_neighbour_ == &dummy_;
    }
    size_t Size() const {
        ListHook* cur = dummy_.right_neighbour_;
        size_t cnt = 0;
        while (cur != &dummy_) {
            cnt++;
            cur = cur[0].right_neighbour_;
        }
        return cnt;
    }

    void PushBack(T* elem) {
        static_cast<ListHook*>(elem)->LinkBefore(&dummy_);
    }  // хочу вставить тут  элемент перед фейковый,беру линкбефор из хука
    void PushFront(T* elem) {
        (*static_cast<ListHook*>(elem)).LinkBefore(dummy_.right_neighbour_);
    }

    T& Front() {
        return *static_cast<T*>(dummy_.right_neighbour_);
    }
    const T& Front() const {
        return *static_cast<const T*>(dummy_.right_neighbour_);
    }

    T& Back() {
        return *static_cast<T*>(dummy_.left_neighbour_);
    }
    const T& Back() const {
        return *static_cast<const T*>(dummy_.left_neighbour_);
    }

    void PopBack() {
        if (!IsEmpty()) {
            (*dummy_.left_neighbour_).Unlink();
        }
    }  // методы листхука заюзаю
    void PopFront() {
        if (!IsEmpty()) {
            (*dummy_.right_neighbour_).Unlink();
        }
    }

    Iterator Begin() {
        return Iterator(dummy_.right_neighbour_);
    }
    Iterator End() {
        return Iterator(&dummy_);
    }

    Iterator IteratorTo(T* element) {
        return Iterator(static_cast<ListHook*>(element));
    }

private:
    ListHook dummy_;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}