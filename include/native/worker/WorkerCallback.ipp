#ifndef __NATIVE_WORKER_WORKERCALLBACK_IPP__
#define __NATIVE_WORKER_WORKERCALLBACK_IPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

template <typename R, typename... Args>
std::shared_ptr<WorkerCallback<R, Args...>>
WorkerCallback<R, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args) {
  return std::shared_ptr<WorkerCallback<R, Args...>>(
      new WorkerCallback<R, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
std::shared_ptr<WorkerCallback<Future<R>, Args...>> WorkerCallback<Future<R>, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args) {
  return std::shared_ptr<WorkerCallback<Future<R>, Args...>>(
      new WorkerCallback<Future<R>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
std::shared_ptr<WorkerCallback<Future<void>, Args...>> WorkerCallback<Future<void>, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args) {
  return std::shared_ptr<WorkerCallback<Future<void>, Args...>>(
      new WorkerCallback<Future<void>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
std::shared_ptr<WorkerCallback<void, Args...>>
WorkerCallback<void, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args) {
  return std::shared_ptr<WorkerCallback<void, Args...>>(
      new WorkerCallback<void, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
WorkerCallback<R, Args...>::WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args)
    : WorkerCallbackBase(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {}

template <typename R, typename... Args>
WorkerCallback<Future<R>, Args...>::WorkerCallback(std::shared_ptr<Loop> iLoop,
                                                   std::function<Future<R>(Args...)> f,
                                                   Args &&... args)
    : WorkerCallbackBase(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {}

template <typename... Args>
WorkerCallback<Future<void>, Args...>::WorkerCallback(std::shared_ptr<Loop> iLoop,
                                                      std::function<Future<void>(Args...)> f,
                                                      Args &&... args)
    : WorkerCallbackBase(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<void>::Create(iLoop)) {
}

template <typename... Args>
WorkerCallback<void, Args...>::WorkerCallback(std::shared_ptr<Loop> iLoop,
                                              std::function<void(Args...)> f,
                                              Args &&... args)
    : WorkerCallbackBase(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<void>::Create(iLoop)) {
}

template <typename R, typename... Args>
std::shared_ptr<WorkerCallback<R, Args...>> WorkerCallback<R, Args...>::getInstance() {
  return std::static_pointer_cast<WorkerCallback<R, Args...>, WorkerBase>(WorkerBase::getInstance());
}

template <typename R, typename... Args>
std::shared_ptr<WorkerCallback<Future<R>, Args...>> WorkerCallback<Future<R>, Args...>::getInstance() {
  return std::static_pointer_cast<WorkerCallback<Future<R>, Args...>, WorkerBase>(WorkerBase::getInstance());
}

template <typename... Args>
std::shared_ptr<WorkerCallback<Future<void>, Args...>> WorkerCallback<Future<void>, Args...>::getInstance() {
  return std::static_pointer_cast<WorkerCallback<Future<void>, Args...>, WorkerBase>(WorkerBase::getInstance());
}

template <typename... Args>
std::shared_ptr<WorkerCallback<void, Args...>> WorkerCallback<void, Args...>::getInstance() {
  return std::static_pointer_cast<WorkerCallback<void, Args...>, WorkerBase>(WorkerBase::getInstance());
}

template <typename R, typename... Args> std::shared_ptr<FutureShared<R>> WorkerCallback<R, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
std::shared_ptr<FutureShared<R>> WorkerCallback<Future<R>, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename... Args> std::shared_ptr<FutureShared<void>> WorkerCallback<Future<void>, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename... Args> std::shared_ptr<FutureShared<void>> WorkerCallback<void, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
template <std::size_t... Is>
void WorkerCallback<R, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<WorkerCallbackBase> iInstance = this->getInstance();

  try {
    this->_resolver =
        std::make_unique<FutureSharedResolverValue<R>>(std::forward<R>(this->_f(std::get<Is>(this->_args)...)));
  } catch (const FutureError &e) {
    this->_resolver = std::make_unique<FutureSharedResolverError<R>>(e);
  }
}

template <typename R, typename... Args>
template <std::size_t... Is>
void WorkerCallback<Future<R>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<WorkerCallback<Future<R>, Args...>> instance = this->getInstance();
  try {
    this->_resolver = std::make_unique<FutureSharedResolverFuture<R>>(this->_f(std::get<Is>(this->_args)...));
  } catch (const FutureError &e) {
    this->_resolver = std::make_unique<FutureSharedResolverError<R>>(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void WorkerCallback<Future<void>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::weak_ptr<WorkerCallback<Future<void>, Args...>> instanceWeak = this->getInstance();
  try {
    this->_resolver = std::make_unique<FutureSharedResolverFuture<void>>(this->_f(std::get<Is>(this->_args)...));
  } catch (const FutureError &e) {
    this->_resolver = std::make_unique<FutureSharedResolverError<void>>(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void WorkerCallback<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<WorkerCallbackBase> iInstance = this->getInstance();

  try {
    this->_f(std::get<Is>(this->_args)...);
    this->_resolver = std::make_unique<FutureSharedResolverValue<void>>();
  } catch (const FutureError &e) {
    this->_resolver = std::make_unique<FutureSharedResolverError<void>>(e);
  }
}

template <typename R, typename... Args> void WorkerCallback<R, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args> void WorkerCallback<Future<R>, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void WorkerCallback<Future<void>, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void WorkerCallback<void, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args> void WorkerCallback<R, Args...>::executeWorkerAfter(int iStatus) {
  /// resolver is populated all the time
  NNATIVE_ASSERT(this->_resolver);
  this->_resolver->resolve(this->getFuture());
}

template <typename R, typename... Args> void WorkerCallback<Future<R>, Args...>::executeWorkerAfter(int iStatus) {
  /// resolver is populated all the time
  NNATIVE_ASSERT(this->_resolver);
  this->_resolver->resolve(this->getFuture());
}

template <typename... Args> void WorkerCallback<Future<void>, Args...>::executeWorkerAfter(int iStatus) {
  /// resolver is populated all the time
  NNATIVE_ASSERT(this->_resolver);
  this->_resolver->resolve(this->getFuture());
}

template <typename... Args> void WorkerCallback<void, Args...>::executeWorkerAfter(int iStatus) {
  /// resolver is populated all the time
  NNATIVE_ASSERT(this->_resolver);
  this->_resolver->resolve(this->getFuture());
}

} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERCALLBACK_IPP__ */
