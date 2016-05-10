#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_IPP__
#define __NATIVE_ASYNC_ACTIONCALLBACK_IPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "./ActionCallback.hpp"
#include "./Future.hpp"

namespace native {

template <typename P> ActionCallbackBase<P>::ActionCallbackBase(std::shared_ptr<Loop> iLoop) : AsyncBase(iLoop) {}

template <typename P> std::shared_ptr<ActionCallbackBase<P>> ActionCallbackBase<P>::getInstance() {
  return std::static_pointer_cast<ActionCallbackBase<P>, AsyncBase>(AsyncBase::getInstance());
}

template <typename P> void ActionCallbackBase<P>::executeAsync() {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_resolver);
  _resolver->resolveCb(this->getInstance());
}

template <typename P> void ActionCallbackBase<P>::closeAsync() {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_resolver);
  _resolver.reset();
}

template <typename P> void ActionCallbackBase<P>::resolve(P p) {
  NNATIVE_ASSERT(!_resolver);
  _resolver = std::make_unique<FutureSharedResolverValue<P>>(std::forward<P>(p));
  this->enqueue();
}

template <typename P> void ActionCallbackBase<P>::reject(const FutureError &iError) {
  NNATIVE_ASSERT(!_resolver);
  _resolver = std::make_unique<FutureSharedResolverError<P>>(iError);
  this->enqueue();
}

template <typename R, typename... Args>
std::shared_ptr<ActionCallback<R, Args...>>
ActionCallback<R, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallback<R, Args...>>(
      new ActionCallback<R, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
std::shared_ptr<ActionCallback<Future<R>, Args...>> ActionCallback<Future<R>, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallback<Future<R>, Args...>>(
      new ActionCallback<Future<R>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
std::shared_ptr<ActionCallback<Future<void>, Args...>> ActionCallback<Future<void>, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallback<Future<void>, Args...>>(
      new ActionCallback<Future<void>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
std::shared_ptr<ActionCallback<void, Args...>>
ActionCallback<void, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallback<void, Args...>>(
      new ActionCallback<void, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<R, P, Args...>>
ActionCallbackP1<R, P, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackP1<R, P, Args...>>(
      new ActionCallbackP1<R, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<Future<R>, P, Args...>> ActionCallbackP1<Future<R>, P, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackP1<Future<R>, P, Args...>>(
      new ActionCallbackP1<Future<R>, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<Future<void>, P, Args...>> ActionCallbackP1<Future<void>, P, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackP1<Future<void>, P, Args...>>(
      new ActionCallbackP1<Future<void>, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename P, typename... Args>
std::shared_ptr<ActionCallbackP1<void, P, Args...>> ActionCallbackP1<void, P, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackP1<void, P, Args...>>(
      new ActionCallbackP1<void, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
ActionCallback<R, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<R>::Create(iLoop)) {}

template <typename R, typename... Args>
ActionCallback<Future<R>, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop,
                                                   std::function<Future<R>(Args...)> f,
                                                   Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<R>::Create(iLoop)) {}

template <typename... Args>
ActionCallback<Future<void>, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop,
                                                      std::function<Future<void>(Args...)> f,
                                                      Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename... Args>
ActionCallback<void, Args...>::ActionCallback(std::shared_ptr<Loop> iLoop,
                                              std::function<void(Args...)> f,
                                              Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename R, typename P, typename... Args>
ActionCallbackP1<R, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop,
                                                  std::function<R(P, Args...)> f,
                                                  Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {
}

template <typename R, typename P, typename... Args>
ActionCallbackP1<Future<R>, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop,
                                                          std::function<Future<R>(P, Args...)> f,
                                                          Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {
}

template <typename P, typename... Args>
ActionCallbackP1<Future<void>, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop,
                                                             std::function<Future<void>(P, Args...)> f,
                                                             Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename P, typename... Args>
ActionCallbackP1<void, P, Args...>::ActionCallbackP1(std::shared_ptr<Loop> iLoop,
                                                     std::function<void(P, Args...)> f,
                                                     Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename R, typename... Args> std::shared_ptr<FutureShared<R>> ActionCallback<R, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallback<Future<R>, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename... Args> std::shared_ptr<FutureShared<void>> ActionCallback<Future<void>, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename... Args> std::shared_ptr<FutureShared<void>> ActionCallback<void, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackP1<R, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackP1<Future<R>, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackP1<Future<void>, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackP1<void, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
template <std::size_t... Is>
void ActionCallback<R, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  try {
    this->getFuture()->resolve(this->_f(std::get<Is>(this->_args)...));
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename... Args>
template <std::size_t... Is>
void ActionCallback<Future<R>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<void>> iInstance = this->getInstance();
  try {
    this->_f(std::get<Is>(this->_args)...)
        .then([iInstance](R &&r) {
          ActionCallback<Future<R>, Args...> *currPtr =
              static_cast<ActionCallback<Future<R>, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve(std::forward<R>(r));
        })
        .error([iInstance](const FutureError &iError) {
          ActionCallback<Future<R>, Args...> *currPtr =
              static_cast<ActionCallback<Future<R>, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void ActionCallback<Future<void>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<void>> iInstance = this->getInstance();
  try {
    this->_f(std::get<Is>(this->_args)...)
        .then([iInstance]() {
          ActionCallback<Future<void>, Args...> *currPtr =
              static_cast<ActionCallback<Future<void>, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve();
        })
        .error([iInstance](const FutureError &iError) {
          ActionCallback<Future<void>, Args...> *currPtr =
              static_cast<ActionCallback<Future<void>, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void ActionCallback<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  try {
    this->_f(std::get<Is>(this->_args)...);
    this->getFuture()->resolve();
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackP1<R, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
  try {
    this->getFuture()->resolve(this->_f(std::forward<P>(p), std::get<Is>(this->_args)...));
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackP1<Future<R>, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<P>> iInstance = this->getInstance();
  try {
    this->_f(std::forward<P>(p), std::get<Is>(this->_args)...)
        .template then([iInstance](R r) {
          ActionCallbackP1<Future<R>, P, Args...> *currPtr =
              static_cast<ActionCallbackP1<Future<R>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve(std::forward<R>(r));
        })
        .template error([iInstance](const FutureError &iError) {
          ActionCallbackP1<Future<R>, P, Args...> *currPtr =
              static_cast<ActionCallbackP1<Future<R>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackP1<Future<void>, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<P>> iInstance = this->getInstance();
  try {
    this->_f(std::forward<P>(p), std::get<Is>(this->_args)...)
        .template then([iInstance]() {
          ActionCallbackP1<Future<void>, P, Args...> *currPtr =
              static_cast<ActionCallbackP1<Future<void>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve();
        })
        .template error([iInstance](const FutureError &iError) {
          ActionCallbackP1<Future<void>, P, Args...> *currPtr =
              static_cast<ActionCallbackP1<Future<void>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackP1<void, P, Args...>::callFn(P p, helper::TemplateSeqInd<Is...>) {
  try {
    this->_f(std::forward<P>(p), std::get<Is>(this->_args)...);
    this->getFuture()->resolve();
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename... Args> void ActionCallback<R, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args> void ActionCallback<Future<R>, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void ActionCallback<Future<void>, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void ActionCallback<void, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename P, typename... Args> void ActionCallbackP1<R, P, Args...>::resolveCb(P p) {
  this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename P, typename... Args> void ActionCallbackP1<Future<R>, P, Args...>::resolveCb(P p) {
  this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args> void ActionCallbackP1<Future<void>, P, Args...>::resolveCb(P p) {
  this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args> void ActionCallbackP1<void, P, Args...>::resolveCb(P p) {
  this->template callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args> void ActionCallback<R, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename R, typename... Args> void ActionCallback<Future<R>, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename... Args> void ActionCallback<Future<void>, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename... Args> void ActionCallback<void, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename R, typename P, typename... Args>
void ActionCallbackP1<R, P, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename R, typename P, typename... Args>
void ActionCallbackP1<Future<R>, P, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename P, typename... Args>
void ActionCallbackP1<Future<void>, P, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

template <typename P, typename... Args> void ActionCallbackP1<void, P, Args...>::rejectCb(const FutureError &iError) {
  this->getFuture()->reject(iError);
}

// Error

template <typename... Args>
std::shared_ptr<ActionCallbackError<Args...>> ActionCallbackError<Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<void(const FutureError &, Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackError<Args...>>(
      new ActionCallbackError<Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
std::shared_ptr<ActionCallbackErrorP1<R, Args...>> ActionCallbackErrorP1<R, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<R(const FutureError &, Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackErrorP1<R, Args...>>(
      new ActionCallbackErrorP1<R, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
ActionCallbackError<Args...>::ActionCallbackError(std::shared_ptr<Loop> iLoop,
                                                  std::function<void(const FutureError &, Args...)> f,
                                                  Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename R, typename... Args>
ActionCallbackErrorP1<R, Args...>::ActionCallbackErrorP1(std::shared_ptr<Loop> iLoop,
                                                         std::function<R(const FutureError &, Args...)> f,
                                                         Args &&... args)
    : ActionCallbackBase<R>(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {
}

template <typename... Args> std::shared_ptr<FutureShared<void>> ActionCallbackError<Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackErrorP1<R, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
template <std::size_t... Is>
void ActionCallbackErrorP1<R, Args...>::callFn(const FutureError &iError, helper::TemplateSeqInd<Is...>) {
  try {
    this->getFuture()->resolve(std::forward<R>(this->_f(iError, std::get<Is>(this->_args)...)));
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void ActionCallbackError<Args...>::callFn(const FutureError &iError, helper::TemplateSeqInd<Is...>) {
  try {
    this->_f(iError, std::get<Is>(this->_args)...);
    this->getFuture()->resolve();
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename... Args> void ActionCallbackError<Args...>::resolveCb() { this->getFuture()->resolve(); }

template <typename R, typename... Args> void ActionCallbackErrorP1<R, Args...>::resolveCb(R r) {
  this->getFuture()->resolve(std::forward<R>(r));
}

template <typename... Args> void ActionCallbackError<Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(iError, helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args> void ActionCallbackErrorP1<P, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(iError, helper::TemplateSeqIndGen<sizeof...(Args)>());
}

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACK_IPP__ */
