#ifndef __NATIVE_ASYNC_FEATURESHARED_H__
#define __NATIVE_ASYNC_FEATURESHARED_H__

#include <vector>
#include <memory>

namespace native {

template<class R>
class FutureShared {
private:
    std::vector<std::unique_ptr<ActionCallbackBase<R>>> _actions;
    bool _satisfied;

public:
    typedef R result_type;

    FutureShared() : _satisfied(false) {}

    void setValue(R&& iVal);
    void setException(const FutureError& iError);

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
    bool _satisfied;
public:
    typedef void result_type;

    FutureShared() : _satisfied(false) {}

    void setValue() {
        if(_satisfied) {
            throw PromiseAlreadySatisfied();
        }

        _satisfied = true;

        for(std::unique_ptr<ActionCallbackBase<void>> &action : this->_actions) {
            action->setValue();
        }
    }

    void setException(const FutureError& iError) {
        if(_satisfied) {
            throw PromiseAlreadySatisfied();
        }

        _satisfied = true;

        for(std::unique_ptr<ActionCallbackBase<void>> &action : this->_actions) {
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
