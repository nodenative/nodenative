#ifndef __NATIVE_ASYNC_FEATURESHARED_I__
#define __NATIVE_ASYNC_FEATURESHARED_I__

namespace native {

template<class R>
void FutureShared<R>::setValue(R&& iVal) {
    for(std::unique_ptr<ActionCallbackBase<R>> &action : this->_actions) {
        action->setValue(std::forward<R>(iVal));
    }
}

template<typename R>
void FutureShared<R>::setException(const exception& iError) {
    for(std::unique_ptr<ActionCallbackBase<R>> &action : this->_actions) {
        action->setException(iError);
    }
}

template<class R>
template<class F, typename... Args>
std::shared_ptr<FutureShared<typename std::result_of<F(R, Args...)>::type>>
FutureShared<R>::then(F&& f, Args&&... args) {
    using return_type = typename std::result_of<F(R, Args...)>::type;
    std::unique_ptr<ActionCallbackP1<return_type, R, Args...>> action(new ActionCallbackP1<return_type, R, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    std::shared_ptr<FutureShared<return_type>> currFuture = action->getFuture();
    _actions.push_back(std::move(action));
    return currFuture;
}

template<class F, typename... Args>
std::shared_ptr<FutureShared<typename std::result_of<F(Args...)>::type>>
FutureShared<void>::then(F&& f, Args&&... args) {
    using return_type = typename std::result_of<F(Args...)>::type;
    std::unique_ptr<ActionCallback<return_type, Args...>> action(new ActionCallback<return_type, Args...>(std::forward<F>(f), std::forward<Args>(args)...));
    std::shared_ptr<FutureShared<return_type>> currFuture = action->getFuture();
    _actions.push_back(std::move(action));
    return currFuture;
}

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_I__
