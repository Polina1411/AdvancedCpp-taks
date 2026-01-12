#pragma once
#include "utf8.h"
#include <optional>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <cstring>
using std::optional;

struct GrepOptions {
    optional<size_t> look_ahead_length;
    size_t max_matches_per_line;

    GrepOptions() {
        max_matches_per_line = 10;
    }

    GrepOptions(size_t look_ahead_length) : GrepOptions() {
        this->look_ahead_length = look_ahead_length;
    }

    GrepOptions(optional<size_t> look_ahead_length, size_t max_matches_per_line) {
        this->look_ahead_length = look_ahead_length;
        this->max_matches_per_line = max_matches_per_line;
    }
};
std::u32string utf8toUtf32(std::string l) {
    std::u32string result;
    utf8::utf8to32(l.begin(), l.end(), std::back_inserter(result));
    return result;
}
std::string utf32toUtf8(std::u32string str) {
    std::string ans;
    utf8::utf32to8(str.begin(), str.end(), std::back_inserter(ans));
    return ans;
}
template <class Visitor>
void Grep(const std::string& path, const std::string& pattern, Visitor visitor,
          const GrepOptions& options) {
    std::error_code ec;
    if (std::filesystem::is_directory(path, ec)) {
        auto iter = std::filesystem::directory_iterator(path, ec);
        if (ec) {
            return;
        }
        for (auto it = begin(iter); it != end(iter); it.increment(ec)) {
            if (ec) {
                return;
            }
            Grep(it->path().string(), pattern, visitor, options);
        }
        return;
    }
    if (!std::filesystem::is_regular_file(path, ec)) {
        return;
    }
    std::ifstream file(path);
    if (!file.is_open()) {
        visitor.OnError(std::strerror(errno));
        return;
    }
    std::string l;
    while (std::getline(file, l)) {
        if (!utf8::is_valid(l.begin(), l.end())) {
            visitor.OnError("value is not utf8");
            return;
        }
    }
    file.clear();
    file.seekg(0);
    size_t line_n = 0;
    while (std::getline(file, l)) {
        ++line_n;
        std::u32string pattern_32 = utf8toUtf32(pattern);
        std::u32string l_32 = utf8toUtf32(l);
        auto it = l_32.begin();
        size_t found = 0;
        auto searcher = std::boyer_moore_searcher(pattern.begin(), pattern.end());
        while (it != l_32.end()) {
            auto patterns = std::search(
                it, l_32.end(), std::boyer_moore_searcher(pattern_32.begin(), pattern_32.end()));
            if (patterns == l_32.end()) {
                break;
            }
            optional<std::string> matsh;
            if (options.look_ahead_length.has_value()) {
                auto start = pattern_32.size() + patterns;
                auto end = start;
                size_t look_ahead = std::min(options.look_ahead_length.value(),
                                             static_cast<size_t>(l_32.end() - start));
                std::advance(end, look_ahead);
                std::u32string afteru32 = std::u32string(start, end);
                matsh = utf32toUtf8(afteru32);
            }
            visitor.OnMatch(path, line_n, patterns - l_32.begin() + 1, matsh);
            it = ++patterns;
            ++found;
            if (found == options.max_matches_per_line) {
                break;
            }
        }
    }
}
