#ifndef __NATIVE_ASYNC_FEATURESHARED_I__
#define __NATIVE_ASYNC_FEATURESHARED_I__

namespace native {

template<class R>
std::shared_ptr<FutureShared<R>> FutureShared<R>::Create(std::shared_ptr<Loop> iLoop) {
    std::shared_ptr<FutureShared<R>> instance(new FutureShared<R>(iLoop));
    instance->setInstance(instance);

    return instance;
}

template<class R>
void FutureShared<R>::setValue(R iVal) {
    NNATIVE_CHECK_LOOP_THREAD(this->_loop);
    if(this->_satisfied) {
        throw PromiseAlreadySatisfied();
    }

    this->_satisfied = true;
    this->_value = std::forward<R>(iVal);

    for(std::shared_ptr<ActionCallbackBase<R>> action : this->_actions) {
        action->SetValue(action, std::forward<R>(iVal));
    }
}

template<typename R>
void FutureShared<R>::setError(const FutureError& iError) {
    NNATIVE_CHECK_LOOP_THREAD(this->_loop);
    if(this->_satisfied) {
        throw PromiseAlreadySatisfied();
    }

    this->_satisfied = true;
    this->_isError = true;
    this->_error = iError;

    for(std::shared_ptr<ActionCallbackBase<R>> action : this->_actions) {
        action->SetError(action, iError);
    }
}

template<typename R>
void FutureShared<R>::setInstance(std::shared_ptr<FutureShared<R>> iInstance) {
    NNATIVE_ASSERT(iInstance && this == iInstance.get());
    _instance = iInstance;
}

template<class R>
template<class F, typename... Args>
std::shared_ptr<FutureShared<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>>
FutureShared<R>::then(F&& f, Args&&... args) {
    NNATIVE_ASSERT(!_instance.expired());

    using return_type = typename std::result_of<F(R, Args...)>::type;
    std::shared_ptr<ActionCallbackP1<return_type, R, Args...>> action(new ActionCallbackP1<return_type, R, Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    auto currFuture = action->getFuture();

    if(!this->_loop->isNotOnEventLoopThread() && !this->_satisfied) {
        _actions.push_back(action);
    } else {
        // avoid race condition
        std::shared_ptr<FutureShared<R>> iInstance = _instance.lock();
        async(this->_loop, [iInstance, action](){
            iInstance->_actions.push_back(action);

            if(iInstance->_satisfied) {
                if(!iInstance->_isError) {
                    // TODO: add post action
                    action->SetValue(action, iInstance->_value.value());
                } else {
                    action->SetError(action, iInstance->_error);
                }
            }
        });
    }

    return currFuture;
}

template<class F, typename... Args>
std::shared_ptr<FutureShared<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
FutureShared<void>::then(F&& f, Args&&... args) {
    NNATIVE_ASSERT(!_instance.expired());

    using return_type = typename std::result_of<F(Args...)>::type;
    std::shared_ptr<ActionCallback<return_type, Args...>> action(new ActionCallback<return_type, Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    auto currFuture = action->getFuture();

    if(!this->_loop->isNotOnEventLoopThread() && !this->_satisfied) {
        _actions.push_back(action);
    } else {
        // avoid race condition
        std::shared_ptr<FutureShared<void>> iInstance = _instance.lock();
        std::shared_ptr<ActionCallback<return_type, Args...>> iAction = action;
        async(this->_loop, [iInstance, action](){
            iInstance->_actions.push_back(action);

            if(iInstance->_satisfied) {
                if(!iInstance->_isError) {
                    action->SetValue(action);
                } else {
                    action->SetError(action, iInstance->_error);
                }
            }
        });
    }
    return currFuture;
}

template<class R>
template<class F, typename... Args>
std::shared_ptr<FutureShared<R>>
FutureShared<R>::error(F&& f, Args&&... args) {
    NNATIVE_ASSERT(!_instance.expired());

    std::shared_ptr<ActionCallbackErrorP1<R, Args...>> action(new ActionCallbackErrorP1<R, Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    std::shared_ptr<FutureShared<R>> currFuture = action->getFuture();

    if(!this->_loop->isNotOnEventLoopThread() && !this->_satisfied) {
        _actions.push_back(action);
    } else {
        // avoid race condition
        std::shared_ptr<FutureShared<R>> iInstance = _instance.lock();
        async(this->_loop, [iInstance, action](){
            iInstance->_actions.push_back(action);

            if(iInstance->_satisfied) {
                if(!iInstance->_isError) {
                    // TODO: add post action
                    //action->SetValue(action, value);
                } else {
                    action->SetError(action, iInstance->_error);
                }
            }
        });
    }

    return currFuture;
}

template<class F, typename... Args>
std::shared_ptr<FutureShared<void>>
FutureShared<void>::error(F&& f, Args&&... args) {
    NNATIVE_ASSERT(!_instance.expired());

    std::shared_ptr<ActionCallbackError<Args...>> action(new ActionCallbackError<Args...>(_loop, std::forward<F>(f), std::forward<Args>(args)...));
    std::shared_ptr<FutureShared<void>> currFuture = action->getFuture();

    if(!this->_loop->isNotOnEventLoopThread() && !this->_satisfied) {
        _actions.push_back(action);
    } else {
        // avoid race condition
        std::shared_ptr<FutureShared<void>> iInstance = _instance.lock();
        async(this->_loop, [iInstance, action](){
            iInstance->_actions.push_back(action);

            if(iInstance->_satisfied) {
                if(!iInstance->_isError) {
                    action->SetValue(action);
                } else {
                    action->SetError(action, iInstance->_error);
                }
            }
        });
    }

    return currFuture;
}

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_I__
