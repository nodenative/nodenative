#ifndef __NATIVE_ASYNC_PROMISE_H__
#define __NATIVE_ASYNC_PROMISE_H__

#include "Future.h"

namespace native {

template<class R>
class Promise {
    Future<R> _future;
public:
    Promise() = delete;
    Promise(std::shared_ptr<Loop> iLoop) : _future(iLoop) {};

    void setValue(R iVal) {
        _future.setValue(std::forward<R>(iVal));
    }

    void setError(const FutureError& iException) {
        _future.setError(iException);
    }

    Future<R> getFuture() { return _future; }
};

template<>
class Promise<void> {
    Future<void> _future;
public:
    Promise() = delete;
    Promise(std::shared_ptr<Loop> iLoop) : _future(iLoop) {};

    void setValue() {
        _future.setValue();
    }

    void setError(const FutureError& iError) {
        _future.setError(iError);
    }

    Future<void> getFuture() { return _future; }
};

} // namespace native

#endif // __NATIVE_ASYNC_PROMISE_H__
