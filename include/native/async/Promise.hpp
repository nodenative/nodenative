#ifndef __NATIVE_ASYNC_PROMISE_HPP__
#define __NATIVE_ASYNC_PROMISE_HPP__

#include "Future.hpp"

namespace native {

template<class R>
class Promise {
    Future<R> _future;
public:
    Promise() = delete;
    Promise(std::shared_ptr<Loop> iLoop) : _future(iLoop) {};

    void resolve(R iVal) {
        _future.resolve(std::forward<R>(iVal));
    }

    void reject(const FutureError& iException) {
        _future.reject(iException);
    }

    void reject(const std::string& iExceptionMsg) {
        FutureError e(iExceptionMsg);
        _future.reject(e);
    }

    void reset() {
        _future = Future<R>(_future.getLoop());
    }

    Future<R> getFuture() { return _future; }

    /** The static method return a Future that is rejected
     * @param iError error message
     */
    static Future<R> Resolve(std::shared_ptr<Loop> iLoop, R r) {
        Promise<R> promise(iLoop);
        promise.resolve(std::forward<R>(r));
        return promise.getFuture();
    }

    /** The static method return a Future that is resolved
     * @param iError error message
     */
    static Future<R> Reject(std::shared_ptr<Loop> iLoop, const std::string& iError) {
        FutureError e(iError);
        return Promise<R>::Reject(iLoop, e);
    }

    static Future<R> Reject(std::shared_ptr<Loop> iLoop, const FutureError& iError) {
        Promise<R> promise(iLoop);
        promise.reject(iError);
        return promise.getFuture();
    }
};

template<>
class Promise<void> {
    Future<void> _future;
public:
    Promise() = delete;
    Promise(std::shared_ptr<Loop> iLoop) : _future(iLoop) {};

    void resolve() {
        _future.resolve();
    }

    void reject(const FutureError& iError) {
        _future.reject(iError);
    }

    void reject(const std::string& iErrorMsg) {
        const FutureError e(iErrorMsg);
        _future.reject(e);
    }

    void reset() {
        _future = Future<void>(_future.getLoop());
    }

    Future<void> getFuture() { return _future; }

    /** The static method return a Future that is rejected
     * @param iError error message
     */
    static Future<void> Resolve(std::shared_ptr<Loop> iLoop) {
        Promise<void> promise(iLoop);
        promise.resolve();
        return promise.getFuture();
    }

    /** The static method return a Future that is resolved
     * @param iError error message
     */
    static Future<void> Reject(std::shared_ptr<Loop> iLoop, const std::string& iError) {
        FutureError e(iError);
        return Promise<void>::Reject(iLoop, e);
    }

    static Future<void> Reject(std::shared_ptr<Loop> iLoop, const FutureError& iError) {
        Promise<void> promise(iLoop);
        promise.reject(iError);
        return promise.getFuture();
    }
};

} // namespace native

#endif // __NATIVE_ASYNC_PROMISE_HPP__
