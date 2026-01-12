#pragma once
#include <list>
#include <string>
#include <unordered_map>
class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

private:
    std::list<std::string> keyslist_;
    std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> hook_;
    size_t max_size_;
};