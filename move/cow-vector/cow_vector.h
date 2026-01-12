#pragma once
#include <vector>
#include <cstdint>
#include <string>

class COWVector {
public:
    COWVector();
    ~COWVector();
    COWVector(const COWVector& other);
    COWVector& operator=(const COWVector& other);
    size_t Size() const;
    void Resize(size_t size);
    const std::string& Get(size_t at);
    const std::string& Back();
    void PushBack(const std::string& value);
    void Set(size_t at, const std::string& value);

private:
    struct State {
        std::vector<std::string> data;
        int ref_count = 0;
    };
    State* state_;
    void CopyState();
};