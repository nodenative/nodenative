#ifndef __NATIVE_ASYNC_ACTIONCALLBACKFINALLY_IPP__
#define __NATIVE_ASYNC_ACTIONCALLBACKFINALLY_IPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "./ActionCallbackFinally.hpp"
#include "./Future.hpp"

namespace native {

template <typename R, typename... Args>
std::shared_ptr<ActionCallbackFinally<R, Args...>>
ActionCallbackFinally<R, Args...>::Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinally<R, Args...>>(
      new ActionCallbackFinally<R, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
std::shared_ptr<ActionCallbackFinally<Future<R>, Args...>> ActionCallbackFinally<Future<R>, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinally<Future<R>, Args...>>(
      new ActionCallbackFinally<Future<R>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
std::shared_ptr<ActionCallbackFinally<Future<void>, Args...>> ActionCallbackFinally<Future<void>, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinally<Future<void>, Args...>>(
      new ActionCallbackFinally<Future<void>, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename... Args>
std::shared_ptr<ActionCallbackFinally<void, Args...>> ActionCallbackFinally<void, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinally<void, Args...>>(
      new ActionCallbackFinally<void, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename P, typename... Args>
std::shared_ptr<ActionCallbackFinallyP1<R, P, Args...>> ActionCallbackFinallyP1<R, P, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinallyP1<R, P, Args...>>(
      new ActionCallbackFinallyP1<R, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename P, typename... Args>
std::shared_ptr<ActionCallbackFinallyP1<Future<R>, P, Args...>> ActionCallbackFinallyP1<Future<R>, P, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinallyP1<Future<R>, P, Args...>>(
      new ActionCallbackFinallyP1<Future<R>, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename P, typename... Args>
std::shared_ptr<ActionCallbackFinallyP1<Future<void>, P, Args...>>
ActionCallbackFinallyP1<Future<void>, P, Args...>::Create(std::shared_ptr<Loop> iLoop,
                                                          std::function<Future<void>(Args...)> f,
                                                          Args &&... args) {
  return std::shared_ptr<ActionCallbackFinallyP1<Future<void>, P, Args...>>(
      new ActionCallbackFinallyP1<Future<void>, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename P, typename... Args>
std::shared_ptr<ActionCallbackFinallyP1<void, P, Args...>> ActionCallbackFinallyP1<void, P, Args...>::Create(
    std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args) {
  return std::shared_ptr<ActionCallbackFinallyP1<void, P, Args...>>(
      new ActionCallbackFinallyP1<void, P, Args...>(iLoop, f, std::forward<Args>(args)...));
}

template <typename R, typename... Args>
ActionCallbackFinally<R, Args...>::ActionCallbackFinally(std::shared_ptr<Loop> iLoop,
                                                         std::function<R(Args...)> f,
                                                         Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<R>::Create(iLoop)) {}

template <typename R, typename... Args>
ActionCallbackFinally<Future<R>, Args...>::ActionCallbackFinally(std::shared_ptr<Loop> iLoop,
                                                                 std::function<Future<R>(Args...)> f,
                                                                 Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<R>::Create(iLoop)) {}

template <typename... Args>
ActionCallbackFinally<Future<void>, Args...>::ActionCallbackFinally(std::shared_ptr<Loop> iLoop,
                                                                    std::function<Future<void>(Args...)> f,
                                                                    Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename... Args>
ActionCallbackFinally<void, Args...>::ActionCallbackFinally(std::shared_ptr<Loop> iLoop,
                                                            std::function<void(Args...)> f,
                                                            Args &&... args)
    : ActionCallbackBase<void>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename R, typename P, typename... Args>
ActionCallbackFinallyP1<R, P, Args...>::ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop,
                                                                std::function<R(Args...)> f,
                                                                Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {
}

template <typename R, typename P, typename... Args>
ActionCallbackFinallyP1<Future<R>, P, Args...>::ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop,
                                                                        std::function<Future<R>(Args...)> f,
                                                                        Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...), _future(FutureShared<R>::Create(iLoop)) {
}

template <typename P, typename... Args>
ActionCallbackFinallyP1<Future<void>, P, Args...>::ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop,
                                                                           std::function<Future<void>(Args...)> f,
                                                                           Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename P, typename... Args>
ActionCallbackFinallyP1<void, P, Args...>::ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop,
                                                                   std::function<void(Args...)> f,
                                                                   Args &&... args)
    : ActionCallbackBase<P>(iLoop), _f(f), _args(std::forward<Args>(args)...),
      _future(FutureShared<void>::Create(iLoop)) {}

template <typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackFinally<R, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackFinally<Future<R>, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackFinally<Future<void>, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename... Args> std::shared_ptr<FutureShared<void>> ActionCallbackFinally<void, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackFinallyP1<R, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename P, typename... Args>
std::shared_ptr<FutureShared<R>> ActionCallbackFinallyP1<Future<R>, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackFinallyP1<Future<void>, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename P, typename... Args>
std::shared_ptr<FutureShared<void>> ActionCallbackFinallyP1<void, P, Args...>::getFuture() {
  NNATIVE_ASSERT(this->_future);
  return this->_future;
}

template <typename R, typename... Args>
template <std::size_t... Is>
void ActionCallbackFinally<R, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  try {
    this->getFuture()->resolve(this->_f(std::get<Is>(this->_args)...));
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename... Args>
template <std::size_t... Is>
void ActionCallbackFinally<Future<R>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<void>> iInstance = this->getInstance();
  try {
    this->_f(std::get<Is>(this->_args)...)
        .template then<std::function<void(R)>>([iInstance](R &&r) {
          ActionCallbackFinally<Future<R>, Args...> *currPtr =
              static_cast<ActionCallbackFinally<Future<R>, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve(std::forward<R>(r));
        })
        .template error<std::function<void(const FutureError &)>>([iInstance](const FutureError &iError) {
          ActionCallbackFinally<Future<R>, Args...> *currPtr =
              static_cast<ActionCallbackFinally<Future<R>, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void ActionCallbackFinally<Future<void>, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<void>> iInstance = this->getInstance();
  try {
    this->_f(std::get<Is>(this->_args)...)
        .template then<std::function<void()>>([iInstance]() {
          ActionCallbackFinally<Future<void>, Args...> *currPtr =
              static_cast<ActionCallbackFinally<Future<void>, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve();
        })
        .template error<std::function<void(const FutureError &)>>([iInstance](const FutureError &iError) {
          ActionCallbackFinally<Future<void>, Args...> *currPtr =
              static_cast<ActionCallbackFinally<Future<void>, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename... Args>
template <std::size_t... Is>
void ActionCallbackFinally<void, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  try {
    this->_f(std::get<Is>(this->_args)...);
    this->getFuture()->resolve();
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackFinallyP1<R, P, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  try {
    this->getFuture()->resolve(this->_f(std::get<Is>(this->_args)...));
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackFinallyP1<Future<R>, P, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<P>> iInstance = this->getInstance();
  try {
    this->_f(std::get<Is>(this->_args)...)
        .template then<std::function<void(R)>>([iInstance](R &&r) {
          ActionCallbackFinallyP1<Future<R>, P, Args...> *currPtr =
              static_cast<ActionCallbackFinallyP1<Future<R>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve(std::forward<R>(r));
        })
        .template error<std::function<void(const FutureError &)>>([iInstance](const FutureError &iError) {
          ActionCallbackFinallyP1<Future<R>, P, Args...> *currPtr =
              static_cast<ActionCallbackFinallyP1<Future<R>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackFinallyP1<Future<void>, P, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  std::shared_ptr<ActionCallbackBase<P>> iInstance = this->getInstance();
  try {
    this->_f(std::get<Is>(this->_args)...)
        .template then<std::function<void()>>([iInstance]() {
          ActionCallbackFinallyP1<Future<void>, P, Args...> *currPtr =
              static_cast<ActionCallbackFinallyP1<Future<void>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->resolve();
        })
        .template error<std::function<void(const FutureError &)>>([iInstance](const FutureError &iError) {
          ActionCallbackFinallyP1<Future<void>, P, Args...> *currPtr =
              static_cast<ActionCallbackFinallyP1<Future<void>, P, Args...> *>(iInstance.get());
          currPtr->getFuture()->reject(iError);
        });
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename P, typename... Args>
template <std::size_t... Is>
void ActionCallbackFinallyP1<void, P, Args...>::callFn(helper::TemplateSeqInd<Is...>) {
  try {
    this->_f(std::get<Is>(this->_args)...);
    this->getFuture()->resolve();
  } catch (const FutureError &e) {
    this->getFuture()->reject(e);
  }
}

template <typename R, typename... Args> void ActionCallbackFinally<R, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args> void ActionCallbackFinally<Future<R>, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void ActionCallbackFinally<Future<void>, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void ActionCallbackFinally<void, Args...>::resolveCb() {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename P, typename... Args> void ActionCallbackFinallyP1<R, P, Args...>::resolveCb(P p) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename P, typename... Args>
void ActionCallbackFinallyP1<Future<R>, P, Args...>::resolveCb(P p) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args> void ActionCallbackFinallyP1<Future<void>, P, Args...>::resolveCb(P p) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args> void ActionCallbackFinallyP1<void, P, Args...>::resolveCb(P p) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args> void ActionCallbackFinally<R, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename... Args>
void ActionCallbackFinally<Future<R>, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void ActionCallbackFinally<Future<void>, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename... Args> void ActionCallbackFinally<void, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename P, typename... Args>
void ActionCallbackFinallyP1<R, P, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename R, typename P, typename... Args>
void ActionCallbackFinallyP1<Future<R>, P, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args>
void ActionCallbackFinallyP1<Future<void>, P, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

template <typename P, typename... Args>
void ActionCallbackFinallyP1<void, P, Args...>::rejectCb(const FutureError &iError) {
  this->template callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
}

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACKFINALLY_IPP__ */
