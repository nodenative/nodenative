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
 * The all data will be destroyed, only if the last Futureobject will be destroyed
 */
template<class R>
class Future {
    friend Promise<R>;

    void setValue(R&& iVal) { _p->setValue(std::forward<R>(iVal)); }
    void setException(const FutureError& iException) { _p->setException(iException); }
public:
    typedef R result_type;

    Future() : _p(std::make_shared<FutureShared<R>>()) {}
    Future(std::shared_ptr<FutureShared<R>> p) : _p(p) {}

    template<class F, typename... Args>
    Future<typename std::result_of<F(R, Args...)>::type>
    then(F&& f, Args&&... args) {
        return Future<typename std::result_of<F(R, Args...)>::type>(_p->template then<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    }

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
    void setException(const FutureError& iException) {_p->setException(iException);}
public:
    typedef void result_type;

    Future() : _p(std::make_shared<FutureShared<void>>()) {}
    Future(std::shared_ptr<FutureShared<void>> p) : _p(p) {}

    template<class F, typename... Args>
    Future<typename std::result_of<F(Args...)>::type>
    then(F&& f, Args&&... args) {
        return Future<typename std::result_of<F(Args...)>::type>(_p->template then<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
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

