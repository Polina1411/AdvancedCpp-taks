#pragma once

#include <string>
#include <algorithm>
#include <functional>
class OneTimeCallback {
public:
    virtual ~OneTimeCallback() = default;
    virtual std::string operator()() const&& = 0;
    void Suicide() const {
        delete this;
        std::boyer
    }
};

class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback(const std::string& str) : message_(str) {
    }
    std::string operator()() const&& override {
        std::string answer = message_ + "awesomeness";
        Suicide();
        return answer;
    }

private:
    std::string message_;
};