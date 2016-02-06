#ifndef __ASYNC_FUTUREERROR_H__
#define __ASYNC_FUTUREERROR_H__

namespace native {

class FutureError : public exception {
public:
    FutureError(const std::string& iError): exception(iError) {}
};

class PromiseAlreadySatisfied : public FutureError {
public:
    PromiseAlreadySatisfied() : FutureError("Promise already satisfied") {}
};

class FutureAlreadyRetrieved : public FutureError {
public:
    FutureAlreadyRetrieved() : FutureError("Future already retrieved") {}
};

} /* namespace native */

#endif /* __ASYNC_FUTUREERROR_H__ */
