#include "cow_vector.h"

COWVector::COWVector() {
    state_ = new State;
    state_->ref_count++;
}
COWVector::~COWVector() {
    state_->ref_count--;
    if (state_->ref_count == 0) {
        delete state_;
    }
}

COWVector::COWVector(const COWVector& other) : state_(other.state_) {
    state_->ref_count++;
}
COWVector& COWVector::operator=(const COWVector& other) {
    --state_->ref_count;
    if (!state_->ref_count) {
        delete state_;
    }
    state_ = other.state_;
    state_->ref_count++;
    return *this;
}

size_t COWVector::Size() const {
    return state_->data.size();
}

void COWVector::Resize(size_t size) {
    CopyState();
    return state_->data.resize(size);
}

const std::string& COWVector::Get(size_t at) {
    return state_->data[at];
}
const std::string& COWVector::Back() {
    return state_->data.back();
}

void COWVector::PushBack(const std::string& value) {
    CopyState();
    state_->data.push_back(value);
}

void COWVector::Set(size_t at, const std::string& value) {
    CopyState();
    state_->data[at] = value;
}

void COWVector::CopyState() {
    if (state_->ref_count == 1) {
        return;
    }
    State* new_state = new State;
    state_->ref_count--;
    new_state->ref_count++;
    new_state->data = state_->data;
    state_ = new_state;
}
