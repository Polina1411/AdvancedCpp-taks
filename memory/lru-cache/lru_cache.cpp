#include "lru_cache.h"

LruCache::LruCache(size_t max_size) {
    max_size_ = max_size;
}

void LruCache::Set(const std::string& key, const std::string& value) {
    if (hook_.find(key) != hook_.end()) {
        keyslist_.erase(hook_[key].second);
        hook_.erase(key);
    }
    keyslist_.push_front(key);
    hook_[keyslist_.front()] = std::make_pair(value, keyslist_.begin());
    if (hook_.size() > max_size_) {
        hook_.erase(keyslist_.back());
        keyslist_.pop_back();
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    if (hook_.find(key) == hook_.end()) {
        return false;
    }
    *value = hook_[key].first;
    keyslist_.erase(hook_[key].second);
    keyslist_.push_front(key);
    hook_[keyslist_.front()].second = keyslist_.begin();
    return true;
}