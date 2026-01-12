#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <sys/fcntl.h>
#include <unistd.h>
#include <vector>

bool StartsWith(std::string_view string, std::string_view text);

bool EndsWith(std::string_view string, std::string_view text);

std::string_view StripPrefix(std::string_view string, std::string_view prefix);

std::string_view StripSuffix(std::string_view string, std::string_view suffix);

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n = std::string_view ::npos);

std::string_view StripAsciiWhitespace(std::string_view s);

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);

std::string ReadN(const std::string& filename, size_t n);

std::string AddSlash(std::string_view path);

std::string_view RemoveSlash(std::string_view path);

std::string_view Dirname(std::string_view path);

std::string_view Basename(std::string_view path);

std::string CollapseSlashes(std::string_view path);

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);

template <typename T>
std::enable_if_t<std::is_integral_v<std::remove_reference_t<T>>, size_t> Size(const T& s) {
    if (s == 0) {
        return 1;
    }
    size_t res = 0;
    if (s < 0) {
        ++res;
    }
    T copy = s;
    while (copy != 0) {
        ++res;
        copy /= 10;
    }
    return res;
}
template <typename T>
std::enable_if_t<!std::is_integral_v<std::remove_reference_t<T>>, size_t> Size(const T& s) {
    std::string_view string = s;
    return string.size();
}

template <typename... Args>
void Cat(std::string& string, const Args&... x);

template <typename Head>
std::enable_if_t<!std::is_integral_v<std::remove_reference_t<Head>>, void> Cat(std::string& s,
                                                                               const Head& val) {
    std::string_view st = val;
    s += st;
}
char ToChar(int digit);

template <typename Head>
std::enable_if_t<std::is_integral_v<std::remove_reference_t<Head>>, void> Cat(std::string& s,
                                                                              const Head& val) {
    if (val < 0) {
        s += '-';
    }

    size_t old_size = s.size();

    Head v = val;
    do {
        s += ToChar(std::abs(static_cast<int>(v % 10)));
        v /= 10;
    } while (v != 0);

    std::reverse(s.begin() + old_size, s.end());
}

template <typename... Args>
void Cat([[maybe_unused]] std::string& s) {
}

template <typename Head, typename... Args>
void Cat(std::string& string, const Head& val, const Args&... x) {
    Cat<Head>(string, val);
    Cat<Args...>(string, x...);
}

template <typename... Args>
std::string StrCat(const Args&... args) {
    size_t size = (0 + ... + Size<Args>(args));
    std::string res;
    res.reserve(size);
    Cat(res, args...);
    return res;
}
