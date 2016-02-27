#ifndef __ASYNC_FUTUREERROR_H__
#define __ASYNC_FUTUREERROR_H__

#include "../error.h"

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

#endif /* __ASYNC_FUTUREERROR_H__ */
