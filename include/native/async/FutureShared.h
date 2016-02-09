#ifndef __NATIVE_ASYNC_FEATURESHARED_H__
#define __NATIVE_ASYNC_FEATURESHARED_H__

#include "../loop.h"
#include "ActionCallback.h"

#include <vector>
#include <memory>
#include <atomic>

namespace native {

template<class R>
class FutureShared {
private:
    std::shared_ptr<uv_loop_t> _loop;
    std::atomic<bool> _satisfied;
    std::vector<std::shared_ptr<ActionCallbackBase<R>>> _actions;

public:
    typedef R result_type;

    FutureShared() = delete;
    FutureShared(const FutureShared<R>&) = delete;
    FutureShared(loop &iLoop) : _loop(iLoop.getShared()), _satisfied(false) {}
    FutureShared(std::shared_ptr<uv_loop_t> iLoop) : _loop(iLoop), _satisfied(false) {}

    void setValue(R&& iVal);
    void setError(const FutureError& iError);
    std::shared_ptr<uv_loop_t> getLoop() { return _loop; }

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>>
    then(F&& f, Args&&... args);

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<R>>
    error(F&& f, Args&&... args);
};

template<>
class FutureShared<void> {
private:
    std::shared_ptr<uv_loop_t> _loop;
    std::atomic<bool> _satisfied;
    std::vector<std::shared_ptr<ActionCallbackBase<void>>> _actions;
public:
    typedef void result_type;

    FutureShared() = delete;
    FutureShared(const FutureShared<void>&) = delete;
    FutureShared(loop &iLoop) : _loop(iLoop.getShared()), _satisfied(false) {}
    FutureShared(std::shared_ptr<uv_loop_t> iLoop) : _loop(iLoop), _satisfied(false) {}

    void setValue();
    void setError(const FutureError& iError);

    std::shared_ptr<uv_loop_t> getLoop() { return _loop; }

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
    then(F&& f, Args&&... args);

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<void>>
    error(F&& f, Args&&... args);
};

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_H__
