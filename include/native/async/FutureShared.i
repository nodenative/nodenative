#ifndef __NATIVE_ASYNC_FEATURESHARED_I__
#define __NATIVE_ASYNC_FEATURESHARED_I__

namespace native {

template<class R>
void FutureShared<R>::setValue(R&& iVal) {
    NNATIVE_ASSERT_MSG(isOnEventloopThread(this->_loop), "Not on the event loop thread");
    bool expected = false;
    if(!this->_satisfied.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
        throw PromiseAlreadySatisfied();
    }

    for(std::shared_ptr<ActionCallbackBase<R>> action : this->_actions) {
        action->SetValue(action, std::forward<R>(iVal));
    }
}

template<typename R>
void FutureShared<R>::setError(const FutureError& iError) {
    NNATIVE_ASSERT_MSG(isOnEventloopThread(this->_loop), "Not on the event loop thread");
    bool expected = false;
    if(!this->_satisfied.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
        throw PromiseAlreadySatisfied();
    }

    for(std::shared_ptr<ActionCallbackBase<R>> action : this->_actions) {
        action->SetError(action, iError);
    }
}

template<class R>
template<class F, typename... Args>
std::shared_ptr<FutureShared<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>>
FutureShared<R>::then(F&& f, Args&&... args) {
    if(this->_satisfied.load(std::memory_order_relaxed)) {
        throw FutureAlreadyRetrieved();
    }

    using return_type = typename std::result_of<F(R, Args...)>::type;
    std::shared_ptr<ActionCallbackP1<return_type, R, Args...>> action(new ActionCallbackP1<return_type, R, Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    _actions.push_back(action);
    return action->getFuture();
}

template<class F, typename... Args>
std::shared_ptr<FutureShared<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
FutureShared<void>::then(F&& f, Args&&... args) {
    if(_satisfied.load(std::memory_order_relaxed)) {
        throw FutureAlreadyRetrieved();
    }

    using return_type = typename std::result_of<F(Args...)>::type;
    std::shared_ptr<ActionCallback<return_type, Args...>> action(new ActionCallback<return_type, Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    _actions.push_back(action);
    return action->getFuture();
}

template<class R>
template<class F, typename... Args>
std::shared_ptr<FutureShared<R>>
FutureShared<R>::error(F&& f, Args&&... args) {
    if(_satisfied.load(std::memory_order_relaxed)) {
        throw FutureAlreadyRetrieved();
    }

    std::shared_ptr<ActionCallbackErrorP1<R, Args...>> action(new ActionCallbackErrorP1<R, Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    std::shared_ptr<FutureShared<R>> currFuture = action->getFuture();
    _actions.push_back(action);
    return currFuture;
}

template<class F, typename... Args>
std::shared_ptr<FutureShared<void>>
FutureShared<void>::error(F&& f, Args&&... args) {
    if(_satisfied.load(std::memory_order_relaxed)) {
        throw FutureAlreadyRetrieved();
    }

    std::shared_ptr<ActionCallbackError<Args...>> action(new ActionCallbackError<Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    std::shared_ptr<FutureShared<void>> currFuture = action->getFuture();
    _actions.push_back(action);
    return currFuture;
}

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_I__
