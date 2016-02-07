#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_I__
#define __NATIVE_ASYNC_ACTIONCALLBACK_I__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

template<typename P>
void ActionCallbackBase<P>::SetValue(std::shared_ptr<ActionCallbackBase<P>> iInstance, P&& p) {
    NNATIVE_ASSERT(iInstance);
    iInstance->setValueCb(std::forward<P>(p));
}

template<typename T>
void ActionCallbackBase<void>::SetValueT(std::shared_ptr<ActionCallbackBase<void>> iInstance) {
    NNATIVE_ASSERT(iInstance);
//    async([](std::shared_ptr<ActionCallbackBase<void>> iInstance){
        iInstance->setValueCb();
//    }, iInstance);
}

template<typename P>
void ActionCallbackBase<P>::SetError(std::shared_ptr<ActionCallbackBase<P>> iInstance, const FutureError &iError) {
    NNATIVE_ASSERT(iInstance);
//    async([](std::shared_ptr<ActionCallbackBase<void>> iInstance, FutureError iError){
        iInstance->setErrorCb(iError);
//    }, iInstance, iError);
}

template<typename T>
void ActionCallbackBase<void>::SetErrorT(std::shared_ptr<ActionCallbackBase<void>> iInstance, const FutureError &iError) {
    NNATIVE_ASSERT(iInstance);
//    async([](std::shared_ptr<ActionCallbackBase<void>> iInstance, FutureError iError){
        iInstance->setErrorCb(iError);
//    }, iInstance, iError);
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallback<R, Args...>::getFuture() {
    if(!this->_future) {
        this->_future = std::make_shared<FutureShared<R>>();
    }

    return this->_future;
}

template<typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallback<void, Args...>::getFuture() {
    if(!this->_future) {
        this->_future = std::make_shared<FutureShared<void>>();
    }

    return this->_future;
}

template<typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackP1<R, P, Args...>::getFuture() {
    if(!this->_future) {
        this->_future = std::make_shared<FutureShared<R>>();
    }

    return this->_future;
}

template<typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackP1<void, P, Args...>::getFuture() {
    if(!this->_future) {
        this->_future = std::make_shared<FutureShared<void>>();
    }

    return this->_future;
}

template<typename R, typename P, typename... Args>
template<std::size_t... Is>
void ActionCallbackP1<R, P, Args...>::callFn(P&& p, helper::TemplateSeqInd<Is...>) {
    if(!this->_future) {
        return;
    }

    try {
        this->_future->setValue(this->_f(std::forward<P>(p), std::get<Is>(this->_args)...));
    } catch (const FutureError &e) {
        this->_future->setError(e);
    }
}

template<typename P, typename... Args>
template<std::size_t... Is>
void ActionCallbackP1<void, P, Args...>::callFn(P&& p, helper::TemplateSeqInd<Is...>) {
    if(!this->_future) {
        return;
    }

    try {
        this->_f(std::forward<P>(p), std::get<Is>(_args)...);
        this->_future->setValue();
    } catch (const FutureError &e) {
        this->_future->setError(e);
    }
}

template<typename R, typename... Args>
template<std::size_t... Is>
void ActionCallback<R, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    if(!this->_future) {
        return;
    }

    try {
        this->_future->setValue(this->_f(std::get<Is>(_args)...));
    } catch (const FutureError &e) {
        this->_future->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void ActionCallback<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    if(!this->_future) {
        return;
    }

    try {
        this->_f(std::get<Is>(_args)...);
        this->_future->setValue();
    } catch (const FutureError &e) {
        this->_future->setError(e);
    }
}

template<typename R, typename... Args>
void ActionCallback<R, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename... Args>
void ActionCallback<void, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<R, P, Args...>::setValueCb(P&& p) {
    this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename P, typename... Args>
void ActionCallbackP1<void, P, Args...>::setValueCb(P&& p) {
    this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename... Args>
void ActionCallback<R, Args...>::setErrorCb(const FutureError &iError) {
    if(this->_future) {
        this->_future->setError(iError);
    }
}

template<typename... Args>
void ActionCallback<void, Args...>::setErrorCb(const FutureError &iError) {
    if(this->_future) {
        this->_future->setError(iError);
    }
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<R, P, Args...>::setErrorCb(const FutureError &iError) {
    if(_future) {
        _future->setError(iError);
    }
}

template<typename P, typename... Args>
void ActionCallbackP1<void, P, Args...>::setErrorCb(const FutureError &iError) {
    if(this->_future) {
        this->_future->setError(iError);
    }
}

// Error

template<typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackError<Args...>::getFuture() {
    if(!this->_future) {
        this->_future = std::make_shared<FutureShared<void>>();
    }

    return this->_future;
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackErrorP1<R, Args...>::getFuture() {
    if(!this->_future) {
        this->_future = std::make_shared<FutureShared<R>>();
    }

    return this->_future;
}

template<typename R, typename... Args>
template<std::size_t... Is>
void ActionCallbackErrorP1<R, Args...>::callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>) {
    if(!this->_future) {
        return;
    }

    try {
        this->_future->setValue(this->_f(iError, std::get<Is>(this->_args)...));
    } catch (const FutureError& e) {
        this->_future->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void ActionCallbackError<Args...>::callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>) {
    if(!this->_future) {
        return;
    }

    try {
        this->_f(iError, std::get<Is>(_args)...);
        this->_future->setValue();
    } catch (const FutureError &e) {
        this->_future->setError(e);
    }
}

template<typename... Args>
void ActionCallbackError<Args...>::setValueCb() {
    if(this->_future) {
        this->_future->setValue();
    }
}

template<typename R, typename... Args>
void ActionCallbackErrorP1<R, Args...>::setValueCb(R&& r) {
    if(_future) {
        _future->setValue(std::forward<R>(r));
    }
}

template<typename... Args>
void ActionCallbackError<Args...>::setErrorCb(const FutureError &iError) {
    this->template callFn(iError, helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename P, typename... Args>
void ActionCallbackErrorP1<P, Args...>::setErrorCb(const FutureError &iError) {
    this->template callFn(iError, helper::TemplateSeqIndGen<sizeof...(Args)>());
}

} /* namespace native */

#endif  /* __NATIVE_ASYNC_ACTIONCALLBACK_I__ */
