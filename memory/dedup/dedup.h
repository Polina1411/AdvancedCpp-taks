#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> result(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        result[i] = std::make_unique<string>(*item);
    }
    return result;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::unordered_map<std::string_view, std::vector<int>> indexes;
    std::vector<std::shared_ptr<string>> result(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        indexes[*item].push_back(i);
    }
    for (const auto& [value, idxs] : indexes) {
        auto value_ptr = std::make_shared<string>(string(value));
        for (int idx : idxs) {
            result[idx] = value_ptr;
        }
    }
    return result;
}
