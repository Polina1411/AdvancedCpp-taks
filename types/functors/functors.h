#pragma once

#include <vector>

template <class Functor>
class ReverseBinaryFunctor {
private:
    Functor f_;

public:
    ReverseBinaryFunctor(Functor &f) : f_(f) {
    }

    template <typename T>
    bool operator()(T &first, T &second) {
        return f_(second, first);
    }
};

template <class Functor>
class ReverseUnaryFunctor {
private:
    Functor f_;

public:
    ReverseUnaryFunctor(Functor &f) : f_(f) {
    }

    template <typename T>
    bool operator()(T &funk) {
        return (!f_(funk));
    }
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}
