#ifndef __NATIVE_ASYNC_PROMISE_H__
#define __NATIVE_ASYNC_PROMISE_H__

#include "Future.h"

namespace native {

template<class R>
class Promise {
    Future<R> _future;
public:
    void setValue(R&& iVal) {
        _future.setValue(std::forward<R>(iVal));
    }

    void setException(const exception& iException) {
        _future.setException(iException);
    }

    Future<R> getFuture() { return _future; }
};

template<>
class Promise<void> {
    Future<void> _future;
public:
    void setValue() {
        _future.setValue();
    }

    void setException(const exception& iError) {
        _future.setException(iError);
    }

    Future<void> getFuture() { return _future; }
};

} // namespace native

#endif // __NATIVE_ASYNC_PROMISE_H__
