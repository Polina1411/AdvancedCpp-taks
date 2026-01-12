#pragma once
#include <mutex>
#include <vector>
#include <atomic>
#include <stdexcept>
#include <list>

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher) {
    }
    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher) {
    }
    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash())
        : hasher_(hasher), size_(0), mutexes_size_(std::max(1, expected_threads_count)) {
        mutexes_ = new std::mutex[mutexes_size_];
        if (expected_size != kUndefinedSize) {
            table_.resize(std::max(1, expected_size));
        } else {
            table_.resize(kDefaultConcurrencyLevel);
        }
    }
    bool Insert(const K& key, const V& value) {
        std::lock_guard<std::mutex> rehash_lock(rehash_mutex_);
        if (size_.load() >= table_.size()) {
            std::vector<std::vector<Node>> new_table(table_.size() * 2);
            for (size_t i = 0; i < table_.size(); ++i) {
                for (const auto& node : table_[i]) {
                    size_t new_index = hasher_(node.key) % new_table.size();
                    new_table[new_index].push_back(node);
                }
            }
            table_ = std::move(new_table);
        }
        size_t index = hasher_(key) % table_.size();
        size_t mutex_index = index % mutexes_size_;
        std::lock_guard<std::mutex> bucket_lock(mutexes_[mutex_index]);
        for (const auto& node : table_[index]) {
            if (node.key == key) {
                return false;
            }
        }
        table_[index].push_back({key, value});
        ++size_;
        return true;
    }
    bool Erase(const K& key) {
        std::lock_guard<std::mutex> rehash_lock(rehash_mutex_);
        size_t index = hasher_(key) % table_.size();
        size_t mutex_index = index % mutexes_size_;
        std::lock_guard<std::mutex> bucket_lock(mutexes_[mutex_index]);
        for (auto it = table_[index].begin(); it != table_[index].end(); ++it) {
            if (it->key == key) {
                table_[index].erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }

    std::pair<bool, V> Find(const K& key) const {
        std::lock_guard<std::mutex> rehash_lock(rehash_mutex_);
        size_t index = hasher_(key) % table_.size();
        size_t mutex_index = index % mutexes_size_;
        std::lock_guard<std::mutex> bucket_lock(mutexes_[mutex_index]);
        for (const auto& node : table_[index]) {
            if (node.key == key) {
                return {true, node.value};
            }
        }
        return {false, V()};
    }
    const V At(const K& key) const {
        auto result = Find(key);
        if (!result.first) {
            throw std::out_of_range("Key not found");
        }
        return result.second;
    }
    void Clear() {
        std::lock_guard<std::mutex> rehash_lock(rehash_mutex_);
        for (size_t i = 0; i < mutexes_size_; ++i) {
            mutexes_[i].lock();
        }
        for (auto& bucket : table_) {
            bucket.clear();
        }
        size_ = 0;
        for (size_t i = 0; i < mutexes_size_; ++i) {
            mutexes_[i].unlock();
        }
    }
    size_t Size() const {
        return size_;
    }
    ~ConcurrentHashMap() {
        delete[] mutexes_;
    }
    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;

private:
    struct Node {
        K key;
        V value;
    };
    mutable std::mutex rehash_mutex_;
    std::vector<std::vector<Node>> table_;
    Hash hasher_;
    std::atomic<size_t> size_;
    std::mutex* mutexes_;
    size_t mutexes_size_;
};
template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;
template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = -1;