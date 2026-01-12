#pragma once
#include <cstring>
#include <string>
#include <algorithm>
class StringView {
private:
    const char* data_;
    std::size_t length_;

public:
    StringView(const std::string& string, size_t start = 0, size_t length = std::string::npos)
        : data_(string.data() + start), length_(std::min(string.size() - start, length)) {
    }
    StringView(const char* str);
    StringView(const char* str, size_t length) : data_(str), length_(length) {
    }
    std::size_t Size() const {
        return length_;
    }
    const char& operator[](std::size_t i) const {
        return data_[i];
    }
};
StringView::StringView(const char* str) : data_(str), length_(strlen(str)) {
}
