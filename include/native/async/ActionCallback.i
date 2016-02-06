#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_I__
#define __NATIVE_ASYNC_ACTIONCALLBACK_I__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * Created By : ionlupascu
 * Creation Date : 26-01-2016
 * Last Modified : Sat 06 Feb 2016 06:13:33 GMT
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

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
        this->_future->setException(e);
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
        this->_future->setException(e);
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
        this->_future->setException(e);
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
        this->_future->setException(e);
    }
}

template<typename R, typename... Args>
void ActionCallback<R, Args...>::setException(const FutureError &iError) {
    if(this->_future) {
        this->_future->setException(iError);
    }
}

template<typename... Args>
void ActionCallback<void, Args...>::setException(const FutureError &iError) {
    if(this->_future) {
        this->_future->setException(iError);
    }
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<R, P, Args...>::setException(const FutureError &iError) {
    if(_future) {
        _future->setException(iError);
    }
}

template<typename P, typename... Args>
void ActionCallbackP1<void, P, Args...>::setException(const FutureError &iError) {
    if(this->_future) {
        this->_future->setException(iError);
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
        this->_future->setException(e);
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
        this->_future->setException(e);
    }
}

template<typename... Args>
void ActionCallbackError<Args...>::setValue() {
    if(this->_future) {
        this->_future->setValue();
    }
}

template<typename R, typename... Args>
void ActionCallbackErrorP1<R, Args...>::setValue(R&& r) {
    if(_future) {
        _future->setValue(std::forward<R>(r));
    }
}

} /* namespace native */

#endif  /* __NATIVE_ASYNC_ACTIONCALLBACK_I__ */
