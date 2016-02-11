#ifndef __NATIVE_ASYNC_FEATURE_H__
#define __NATIVE_ASYNC_FEATURE_H__

#include "FutureShared.h"
#include <memory>

/** provide a mechanism to access the result of asynchronous operations:
 * * An asynchronous operation 
 */

namespace native {

template<class R>
class Promise;

/**
 * Future class. If the Future object is copied, the shared data will be the same.
 * All data will be destroyed, only if the last Future object will be destroyed
 */
template<class R>
class Future {
    friend Promise<R>;

    void setValue(R&& iVal) { _p->setValue(std::forward<R>(iVal)); }
    void setError(const FutureError& iException) { _p->setError(iException); }
public:
    /// Result type for the future
    typedef R result_type;

    Future() = delete;

    /** Constructor from Loop object
     */
    Future(loop &iLoop) : _p(FutureShared<R>::Create(iLoop)) {}

    /** Constructor from FutureShared object
     */
    Future(std::shared_ptr<FutureShared<R>> p) : _p(p) {}

    /** Add an waiter callback for value. It will be called if the Future is resolved with a value and it will be skipped for error.
     *
     * An waiter callback may return a value or a Future value. To reject a state, it may throw an error.
     * If a FutureError is thrown, then all dependencies will be considered as error.
     * If it return a future value, the flow will continue only after the future value will be resolved
     * @param f waiter (function or function object) callback, F(R, Args...). First argument is reserver for the current result value, R.
     * @param Args... static arguments
     */
    template<class F, typename... Args>
    Future<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>
    then(F&& f, Args&&... args) {
        return Future<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>(_p->template then<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }

    /** Add an waiter callback for error. It will be called if the future is resolved with an error.
     *
     * An waiter callback will return a value or throw another FutureError.
     * If it will return a value, then the status will be resolved with a value.
     * An error callback cannot resolve with other type of value than R.
     */
    template<class F, typename... Args>
    Future<R>
    error(F&& f, Args&&... args) {
        return Future<R>(_p->template error<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }
private:
    std::shared_ptr<FutureShared<R>> _p;
};

template<>
class Future<void> {
    friend Promise<void>;

    void setValue() {_p->setValue();}
    void setError(const FutureError& iException) {_p->setError(iException);}
public:
    typedef void result_type;

    Future() = delete;
    Future(loop &iLoop) : _p(FutureShared<void>::Create(iLoop)) {}
    Future(std::shared_ptr<FutureShared<void>> p) : _p(p) {}

    template<class F, typename... Args>
    Future<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>
    then(F&& f, Args&&... args) {
        return Future<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>(_p->template then<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }

    template<class F, typename... Args>
    Future<void>
    error(F&& f, Args&&... args) {
        return Future<void>(_p->template error<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }
private:
    std::shared_ptr<FutureShared<void>> _p;
};

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURE_H__

