#include "string_operations.h"

bool StartsWith(std::string_view string, std::string_view text) {
    if (string.size() < text.size()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (string[i] != text[i]) {
            return false;
        }
    }
    return true;
}

bool EndsWith(std::string_view string, std::string_view text) {
    if (string.size() < text.size()) {
        return false;
    }
    return string.compare(string.length() - text.length(), text.length(), text) == 0;
}

std::string_view StripPrefix(std::string_view string, std::string_view prefix) {
    if (!StartsWith(string, prefix)) {
        return string;
    }
    return (string.substr(prefix.size()));
}

std::string_view StripSuffix(std::string_view string, std::string_view suffix) {
    if (!EndsWith(string, suffix)) {
        return string;
    }
    return (string.substr(0, string.size() - suffix.size()));
}

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n) {
    if (n > s.size()) {
        return s;
    }
    return s.substr(pos, n);
}

std::string_view StripAsciiWhitespace(std::string_view s) {
    size_t start = 0;
    while (start < s.size() && isspace(s[start])) {
        start++;
    }
    int end = s.size();
    while (end > 0 && isspace(s[end - 1])) {
        end--;
    }
    return s.substr(start, end - start);
}

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    int st = 1;
    size_t same = 0;
    int beg = 0;
    std::vector<std::string_view> ans;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == delim[same]) {
            ++same;
            if (same == delim.size()) {
                ++st;
                same = 0;
            }
        }
    }
    ans.reserve(st);
    same = 0;
    beg = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == delim[same]) {
            ++same;
            if (same == delim.size()) {
                ans.push_back(std::string_view(text.data() + beg, i - beg - delim.size() + 1));
                beg = 1 + i;
                same = 0;
            }
        } else {
            same = 0;
        }
    }
    ans.push_back(std::string_view(text.data() + beg, text.size() - beg));
    return ans;
}
std::string ReadN(const std::string& filename, size_t n) {
    int fd = open(filename.data(), O_RDONLY);
    if (fd == -1) {
        return {};
    }
    std::string answer;
    answer.resize(n);
    int res = read(fd, answer.data(), n);
    if (res == -1) {
        return {};
    }
    close(fd);
    return answer;
}

std::string AddSlash(std::string_view path) {
    std::string answer;
    answer.reserve(path.size() + ((path.empty() || path.back() != '/') ? 1 : 0));
    answer = path;
    if (path.empty() || path.back() != '/') {
        answer += '/';
    }
    return answer;
}

std::string_view RemoveSlash(std::string_view path) {
    if (path.size() > 1 && path.back() == '/') {
        path.remove_suffix(1);
    }
    return path;
}

std::string_view Dirname(std::string_view path) {
    size_t suffix_len = 0;
    for (auto it = path.rbegin(); it != path.rend() && *it != '/'; ++it) {
        ++suffix_len;
    }

    path.remove_suffix(std::min(suffix_len + 1, path.size() - 1));

    return path;
}

std::string_view Basename(std::string_view path) {
    auto it = path.rbegin();
    while (it != path.rend() && *it != '/') {
        ++it;
    }
    if (it == path.rend()) {
        return path;
    }
    return std::string_view(it.base(), path.end() - it.base());
}

std::string CollapseSlashes(std::string_view path) {
    size_t extra_slashes_count = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i] == '/' && path[i - 1] == '/') {
            ++extra_slashes_count;
        }
    }

    std::string answer;
    answer.reserve(path.size() - extra_slashes_count);
    for (size_t i = 0; i < path.size(); ++i) {
        if (i == 0 || path[i] != '/' || path[i - 1] != '/') {
            answer.push_back(path[i]);
        }
    }

    return answer;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return {};
    }

    size_t answer_size = 0;
    for (const auto& string : strings) {
        answer_size += string.size();
    }
    answer_size += delimiter.size() * (strings.size() - 1);

    std::string answer;
    answer.reserve(answer_size);
    answer += strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        answer += delimiter;
        answer += strings[i];
    }

    return answer;
}

char ToChar(int digit) {
    return '0' + digit;
}
