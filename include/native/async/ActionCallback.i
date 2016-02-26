#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_I__
#define __NATIVE_ASYNC_ACTIONCALLBACK_I__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

template<typename P>
ActionCallbackBaseDetached<P>::ActionCallbackBaseDetached(std::shared_ptr<ActionCallbackBase<P>> iInstance, P p) :
        AsyncBase(iInstance->getLoop()),
        _instance(iInstance),
        _args(p) {
}

template<typename P>
ActionCallbackBaseDetachedError<P>::ActionCallbackBaseDetachedError(std::shared_ptr<ActionCallbackBase<P>> iInstance, const FutureError &iError) :
        AsyncBase(iInstance->getLoop()),
        _instance(iInstance),
        _error(iError) {
}

template<typename P>
void ActionCallbackBaseDetached<P>::executeAsync() {
    NNATIVE_FCALL();
    _instance->setValueCb(std::forward<P>(std::get<0>(_args)));
}


template<typename P>
void ActionCallbackBaseDetachedError<P>::executeAsync() {
    NNATIVE_FCALL();
    _instance->setErrorCb(_error);
}

template<typename P>
void ActionCallbackBaseDetached<P>::Enqueue(std::shared_ptr<ActionCallbackBase<P>> iInstance, P p) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iInstance);
    std::unique_ptr<ActionCallbackBaseDetached<P>> detachedInst(new ActionCallbackBaseDetached<P>(iInstance, std::forward<P>(p)));
    detachedInst->enqueue();
    detachedInst.release();
}

template<typename P>
void ActionCallbackBaseDetachedError<P>::Enqueue(std::shared_ptr<ActionCallbackBase<P>> iInstance, const FutureError &iError) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iInstance);
    std::unique_ptr<ActionCallbackBaseDetachedError<P>> detachedInst(new ActionCallbackBaseDetachedError<P>(iInstance, iError));
    detachedInst->enqueue();
    detachedInst.release();
}

template<typename P>
void ActionCallbackBase<P>::setValue(P p) {
    ActionCallbackBaseDetached<P>::Enqueue(this->shared_from_this(), std::forward<P>(p));
}

template<typename P>
void ActionCallbackBase<P>::setError(const FutureError &iError) {
    ActionCallbackBaseDetachedError<P>::Enqueue(this->shared_from_this(), iError);
}

template<typename R, typename... Args>
std::shared_ptr<ActionCallback<R, Args...>> ActionCallback<R, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallback<R, Args...>>(new ActionCallback<R, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename R, typename... Args>
std::shared_ptr<ActionCallback<Future<R>, Args...>> ActionCallback<Future<R>, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallback<Future<R>, Args...>>(new ActionCallback<Future<R>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename... Args>
std::shared_ptr<ActionCallback<Future<void>, Args...>> ActionCallback<Future<void>, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallback<Future<void>, Args...>>(new ActionCallback<Future<void>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename... Args>
std::shared_ptr<ActionCallback<void, Args...>> ActionCallback<void, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallback<void, Args...>>(new ActionCallback<void, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename R, typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<R, P, Args...>> ActionCallbackP1<R, P, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallbackP1<R, P, Args...>>(new ActionCallbackP1<R, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename R, typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<Future<R>, P, Args...>> ActionCallbackP1<Future<R>, P, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallbackP1<Future<R>, P, Args...>>(new ActionCallbackP1<Future<R>, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<Future<void>, P, Args...>> ActionCallbackP1<Future<void>, P, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallbackP1<Future<void>, P, Args...>>(new ActionCallbackP1<Future<void>, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<void, P, Args...>> ActionCallbackP1<void, P, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallbackP1<void, P, Args...>>(new ActionCallbackP1<void, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename R, typename... Args>
ActionCallback<R, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename R, typename... Args>
ActionCallback<Future<R>, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename... Args>
ActionCallback<Future<void>, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
{
}

template<typename... Args>
ActionCallback<void, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
{
}

template<typename R, typename P, typename... Args>
ActionCallbackP1<R, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename R, typename P, typename... Args>
ActionCallbackP1<Future<R>, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename P, typename... Args>
ActionCallbackP1<Future<void>, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
{
}

template<typename P, typename... Args>
ActionCallbackP1<void, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
{
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallback<R, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallback<Future<R>, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallback<Future<void>, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallback<void, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackP1<R, P, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackP1<Future<R>, P, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackP1<Future<void>, P, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackP1<void, P, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename... Args>
std::shared_ptr<Loop> ActionCallback<R, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename... Args>
std::shared_ptr<Loop> ActionCallback<Future<R>, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename... Args>
std::shared_ptr<Loop> ActionCallback<Future<void>, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename... Args>
std::shared_ptr<Loop> ActionCallback<void, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename P, typename... Args>
std::shared_ptr<Loop> ActionCallbackP1<R, P, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename P, typename... Args>
std::shared_ptr<Loop> ActionCallbackP1<Future<R>, P, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename P, typename... Args>
std::shared_ptr<Loop> ActionCallbackP1<Future<void>, P, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename P, typename... Args>
std::shared_ptr<Loop> ActionCallbackP1<void, P, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename... Args>
template<std::size_t... Is>
void ActionCallback<R, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    try {
        this->getFuture()->setValue(this->_f(std::get<Is>(this->_args)...));
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename R, typename... Args>
template<std::size_t... Is>
void ActionCallback<Future<R>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    std::shared_ptr<ActionCallbackBase<void>> iInstance = this->shared_from_this();
    try {
        this->_f(std::get<Is>(this->_args)...)
            .then([iInstance](R&& r) {
                ActionCallback<Future<R>, Args...> *currPtr = static_cast<ActionCallback<Future<R>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setValue(std::forward<R>(r));
            })
            .error([iInstance](const FutureError& iError){
                ActionCallback<Future<R>, Args...> *currPtr = static_cast<ActionCallback<Future<R>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setError(iError);
            });
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void ActionCallback<Future<void>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    std::shared_ptr<ActionCallbackBase<void>> iInstance = this->shared_from_this();
    try {
        this->_f(std::get<Is>(this->_args)...)
            .then([iInstance]() {
                ActionCallback<Future<void>, Args...> *currPtr = static_cast<ActionCallback<Future<void>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setValue();
            })
            .error([iInstance](const FutureError& iError){
                ActionCallback<Future<void>, Args...> *currPtr = static_cast<ActionCallback<Future<void>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setError(iError);
            });
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void ActionCallback<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    try {
        this->_f(std::get<Is>(this->_args)...);
        this->getFuture()->setValue();
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename R, typename P, typename... Args>
template<std::size_t... Is>
void ActionCallbackP1<R, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
    try {
        this->getFuture()->setValue(this->_f(std::forward<P>(p), std::get<Is>(this->_args)...));
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename R, typename P, typename... Args>
template<std::size_t... Is>
void ActionCallbackP1<Future<R>, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
    std::shared_ptr<ActionCallbackBase<P>> iInstance = this->shared_from_this();
    try {
        this->_f(std::forward<P>(p), std::get<Is>(this->_args)...)
            .template then([iInstance](R r) {
                ActionCallbackP1<Future<R>, P, Args...> *currPtr = static_cast<ActionCallbackP1<Future<R>, P, Args...>*>(iInstance.get());
                currPtr->getFuture()->setValue(std::forward<R>(r));
            })
            .template error([iInstance](const FutureError& iError){
                ActionCallbackP1<Future<R>, P, Args...> *currPtr = static_cast<ActionCallbackP1<Future<R>, P, Args...>*>(iInstance.get());
                currPtr->getFuture()->setError(iError);
            });
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename P, typename... Args>
template<std::size_t... Is>
void ActionCallbackP1<Future<void>, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
    std::shared_ptr<ActionCallbackBase<P>> iInstance = this->shared_from_this();
    try {
        this->_f(std::forward<P>(p), std::get<Is>(this->_args)...)
            .template then([iInstance]() {
                ActionCallbackP1<Future<void>, P, Args...> *currPtr = static_cast<ActionCallbackP1<Future<void>, P, Args...>*>(iInstance.get());
                currPtr->getFuture()->setValue();
            })
            .template error([iInstance](const FutureError& iError){
                ActionCallbackP1<Future<void>, P, Args...> *currPtr = static_cast<ActionCallbackP1<Future<void>, P, Args...>*>(iInstance.get());
                currPtr->getFuture()->setError(iError);
            });
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename P, typename... Args>
template<std::size_t... Is>
void ActionCallbackP1<void, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
    try {
        this->_f(std::forward<P>(p), std::get<Is>(this->_args)...);
        this->getFuture()->setValue();
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename R, typename... Args>
void ActionCallback<R, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename... Args>
void ActionCallback<Future<R>, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename... Args>
void ActionCallback<Future<void>, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename... Args>
void ActionCallback<void, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<R, P, Args...>::setValueCb(P p) {
    this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<Future<R>, P, Args...>::setValueCb(P p) {
    this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename P, typename... Args>
void ActionCallbackP1<Future<void>, P, Args...>::setValueCb(P p) {
    this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename P, typename... Args>
void ActionCallbackP1<void, P, Args...>::setValueCb(P p) {
    this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename... Args>
void ActionCallback<R, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename R, typename... Args>
void ActionCallback<Future<R>, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename... Args>
void ActionCallback<Future<void>, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename... Args>
void ActionCallback<void, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<R, P, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename R, typename P, typename... Args>
void ActionCallbackP1<Future<R>, P, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename P, typename... Args>
void ActionCallbackP1<Future<void>, P, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename P, typename... Args>
void ActionCallbackP1<void, P, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

// Error

template<typename... Args>
std::shared_ptr<ActionCallbackError<Args...>> ActionCallbackError<Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<void(const FutureError&, Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallbackError<Args...>>(new ActionCallbackError<Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename R, typename... Args>
std::shared_ptr<ActionCallbackErrorP1<R, Args...>> ActionCallbackErrorP1<R, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError&, Args...)> f, Args&&... args)
{
    return std::shared_ptr<ActionCallbackErrorP1<R, Args...>>(new ActionCallbackErrorP1<R, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template<typename... Args>
ActionCallbackError<Args...>::ActionCallbackError(std::shared_ptr<Loop> iLoop, std::function<void(const FutureError&, Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
{
}

template<typename R, typename... Args>
ActionCallbackErrorP1<R, Args...>::ActionCallbackErrorP1(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError&, Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackError<Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackErrorP1<R, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename... Args>
std::shared_ptr<Loop> ActionCallbackError<Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename... Args>
std::shared_ptr<Loop> ActionCallbackErrorP1<R, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename... Args>
template<std::size_t... Is>
void ActionCallbackErrorP1<R, Args...>::callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>) {
    try {
        this->getFuture()->setValue(this->_f(iError, std::get<Is>(this->_args)...));
    } catch (const FutureError& e) {
        this->getFuture()->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void ActionCallbackError<Args...>::callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>) {
    try {
        this->_f(iError, std::get<Is>(this->_args)...);
        this->getFuture()->setValue();
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename... Args>
void ActionCallbackError<Args...>::setValueCb() {
    this->getFuture()->setValue();
}

template<typename R, typename... Args>
void ActionCallbackErrorP1<R, Args...>::setValueCb(R r) {
    this->getFuture()->setValue(std::forward<R>(r));
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
