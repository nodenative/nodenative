#ifndef __NATIVE_WORKER_WORKERCALLBACK_I__
#define __NATIVE_WORKER_WORKERCALLBACK_I__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

template<typename R, typename... Args>
WorkerCallback<R, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<R(Args...)> f, Args&&... args) :
         _f(f),
         _args(args...),
         _future(std::make_shared<FutureShared<R>>(iLoop))
{
}

template<typename R, typename... Args>
WorkerCallback<Future<R>, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<Future<R>(Args...)> f, Args&&... args) :
         _f(f),
         _args(args...),
         _future(std::make_shared<FutureShared<R>>(iLoop))
{
}

template<typename... Args>
WorkerCallback<Future<void>, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<Future<void>(Args...)> f, Args&&... args) :
         _f(f),
         _args(args...),
         _future(std::make_shared<FutureShared<void>>(iLoop))
{
}

template<typename... Args>
WorkerCallback<void, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<void(Args...)> f, Args&&... args) :
         _f(f),
         _args(args...),
         _future(std::make_shared<FutureShared<void>>(iLoop))
{
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> WorkerCallback<R, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename... Args>
std::shared_ptr<FutureShared<R>> WorkerCallback<Future<R>, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename... Args>
std::shared_ptr<FutureShared<void>> WorkerCallback<Future<void>, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename... Args>
std::shared_ptr<FutureShared<void>> WorkerCallback<void, Args...>::getFuture() {
    NNATIVE_ASSERT(this->_future);
    return this->_future;
}

template<typename R, typename... Args>
std::shared_ptr<uv_loop_t> WorkerCallback<R, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename... Args>
std::shared_ptr<uv_loop_t> WorkerCallback<Future<R>, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename... Args>
std::shared_ptr<uv_loop_t> WorkerCallback<Future<void>, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename... Args>
std::shared_ptr<uv_loop_t> WorkerCallback<void, Args...>::getLoop() {
    return this->getFuture()->getLoop();
}

template<typename R, typename... Args>
template<std::size_t... Is>
void WorkerCallback<R, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    try {
        this->getFuture()->setValue(this->_f(std::get<Is>(_args)...));
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename R, typename... Args>
template<std::size_t... Is>
void WorkerCallback<Future<R>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    try {
        Future<R> future = this->_f(std::get<Is>(_args)...);
        NNATIVE_ASSERT(!this->_instance.expired());
        future.then([](R&& r, std::shared_ptr<WorkerCallbackBase> iInstance) {
            WorkerCallback<Future<R>, Args...> *currPtr = static_cast<WorkerCallback<Future<R>, Args...>*>(iInstance.get());
            currPtr->getFuture()->setValue(std::forward<R>(r));
        }, this->_instance.lock());
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void WorkerCallback<Future<void>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    try {
        Future<void> future = this->_f(std::get<Is>(_args)...);
        NNATIVE_ASSERT(!this->_instance.expired());
        future.then([](std::shared_ptr<WorkerCallbackBase> iInstance) {
            WorkerCallback<Future<void>, Args...> *currPtr = static_cast<WorkerCallback<Future<void>, Args...>*>(iInstance.get());
            currPtr->getFuture()->setValue();
        }, this->_instance.lock());
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void WorkerCallback<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    try {
        this->_f(std::get<Is>(_args)...);
        this->getFuture()->setValue();
    } catch (const FutureError &e) {
        this->getFuture()->setError(e);
    }
}

template<typename R, typename... Args>
void WorkerCallback<R, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename... Args>
void WorkerCallback<Future<R>, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename... Args>
void WorkerCallback<Future<void>, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename... Args>
void WorkerCallback<void, Args...>::setValueCb() {
    this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template<typename R, typename... Args>
void WorkerCallback<R, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename R, typename... Args>
void WorkerCallback<Future<R>, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename... Args>
void WorkerCallback<Future<void>, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}

template<typename... Args>
void WorkerCallback<void, Args...>::setErrorCb(const FutureError &iError) {
    this->getFuture()->setError(iError);
}


} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERCALLBACK_I__ */

