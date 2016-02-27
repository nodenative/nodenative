#ifndef __ASYNC_FUTUREERROR_HPP__
#define __ASYNC_FUTUREERROR_HPP__

#include "../error.hpp"

namespace native {

class FutureError : public exception {
public:
    FutureError(const std::string& iError): exception(iError) {}
};

class PromiseAlreadySatisfied : public FutureError {
public:
    PromiseAlreadySatisfied() : FutureError("Promise already satisfied") {}
};

} /* namespace native */

#endif /* __ASYNC_FUTUREERROR_HPP__ */
