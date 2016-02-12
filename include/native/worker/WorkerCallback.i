#ifndef __NATIVE_WORKER_WORKERCALLBACK_I__
#define __NATIVE_WORKER_WORKERCALLBACK_I__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

template<typename R, typename... Args>
WorkerCallback<R, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<R(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename R, typename... Args>
WorkerCallback<Future<R>, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<Future<R>(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<R>::Create(iLoop))
{
}

template<typename... Args>
WorkerCallback<Future<void>, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<Future<void>(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
{
}

template<typename... Args>
WorkerCallback<void, Args...>::WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<void(Args...)> f, Args&&... args) :
         _f(f),
         _args(std::forward<Args>(args)...),
         _future(FutureShared<void>::Create(iLoop))
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
    loop currloop(this->_future->getLoop());
    NNATIVE_ASSERT(!this->_instance.expired());
    std::shared_ptr<WorkerCallbackBase> iInstance = this->_instance.lock();

    try {
        async(currloop, [iInstance](R&& r){
            WorkerCallback<R, Args...> *currPtr = static_cast<WorkerCallback<R, Args...>*>(iInstance.get());
            currPtr->getFuture()->setValue(std::forward<R>(r));
        }, std::forward<R>(this->_f(std::get<Is>(this->_args)...)));
    } catch (const FutureError &e) {
        async(currloop, [iInstance, e](){
            WorkerCallback<R, Args...> *currPtr = static_cast<WorkerCallback<R, Args...>*>(iInstance.get());
            currPtr->getFuture()->setError(e);
        });
    }
}

template<typename R, typename... Args>
template<std::size_t... Is>
void WorkerCallback<Future<R>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    NNATIVE_ASSERT(!this->_instance.expired());
    std::shared_ptr<WorkerCallbackBase> iInstance = this->_instance.lock();
    try {
        this->_f(std::get<Is>(this->_args)...)
            .template then([iInstance](R&& r) {
                WorkerCallback<Future<R>, Args...> *currPtr = static_cast<WorkerCallback<Future<R>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setValue(std::forward<R>(r));
            })
            .template error([iInstance](const FutureError &iError){
                WorkerCallback<Future<R>, Args...> *currPtr = static_cast<WorkerCallback<Future<R>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setError(iError);
            });
    } catch (const FutureError &e) {
        loop currloop(this->_future->getLoop());
        async(currloop, [iInstance](FutureError iError){
            WorkerCallback<Future<R>, Args...> *currPtr = static_cast<WorkerCallback<Future<R>, Args...>*>(iInstance.get());
            currPtr->getFuture()->setError(iError);
        }, e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void WorkerCallback<Future<void>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    NNATIVE_ASSERT(!this->_instance.expired());
    std::shared_ptr<WorkerCallbackBase> iInstance = this->_instance.lock();
    try {
        this->_f(std::get<Is>(this->_args)...)
            .template then([iInstance]() {
                WorkerCallback<Future<void>, Args...> *currPtr = static_cast<WorkerCallback<Future<void>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setValue();
            })
            .template error([iInstance](const FutureError &iError){
                WorkerCallback<Future<void>, Args...> *currPtr = static_cast<WorkerCallback<Future<void>, Args...>*>(iInstance.get());
                currPtr->getFuture()->setError(iError);
            });
    } catch (const FutureError &e) {
        loop currloop(this->_future->getLoop());
        async(currloop, [iInstance](FutureError iError){
            WorkerCallback<Future<void>, Args...> *currPtr = static_cast<WorkerCallback<Future<void>, Args...>*>(iInstance.get());
            currPtr->getFuture()->setError(iError);
        }, e);
    }
}

template<typename... Args>
template<std::size_t... Is>
void WorkerCallback<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
    loop currloop(this->_future->getLoop());
    NNATIVE_ASSERT(!this->_instance.expired());
    std::shared_ptr<WorkerCallbackBase> iInstance = this->_instance.lock();

    try {
        this->_f(std::get<Is>(this->_args)...);

        async(currloop, [iInstance](){
            WorkerCallback<void, Args...> *currPtr = static_cast<WorkerCallback<void, Args...>*>(iInstance.get());
            currPtr->getFuture()->setValue();
        });
    } catch (const FutureError &e) {
        async(currloop, [iInstance, e](){
            WorkerCallback<void, Args...> *currPtr = static_cast<WorkerCallback<void, Args...>*>(iInstance.get());
            currPtr->getFuture()->setError(e);
        });
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

