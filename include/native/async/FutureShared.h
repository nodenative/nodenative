#ifndef __NATIVE_ASYNC_FEATURESHARED_H__
#define __NATIVE_ASYNC_FEATURESHARED_H__

#include "../error.h"
#include "ActionCallback.h"

#include <vector>
#include <memory>

namespace native {

template<class R>
class FutureShared {
private:
    std::vector<std::unique_ptr<ActionCallbackBase<R>>> _actions;
public:
    typedef R result_type;

    void setValue(R&& iVal);
    void setException(const exception& iError);

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<typename std::result_of<F(R, Args...)>::type>>
    then(F&& f, Args&&... args);

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<R>>
    error(F&& f, Args&&... args);
};

template<>
class FutureShared<void> {
private:
    std::vector<std::unique_ptr<ActionCallbackBase<void>>> _actions;
public:
    typedef void result_type;

    void setValue() {
        for(std::unique_ptr<ActionCallbackBase<void>> &action : _actions) {
            action->setValue();
        }
    }

    void setException(const exception& iError) {
        for(std::unique_ptr<ActionCallbackBase<void>> &action : _actions) {
            action->setException(iError);
        }
    }

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<typename std::result_of<F(Args...)>::type>>
    then(F&& f, Args&&... args);

    template<class F, typename... Args>
    std::shared_ptr<FutureShared<void>>
    error(F&& f, Args&&... args);
};


} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_H__
