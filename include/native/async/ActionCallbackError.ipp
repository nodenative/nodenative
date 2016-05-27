#ifndef __NATIVE_ASYNC_ACTIONCALLBACKERROR_IPP__
#define __NATIVE_ASYNC_ACTIONCALLBACKERROR_IPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "./ActionCallbackError.hpp"
#include "./Future.hpp"

namespace native {

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

#endif /* __NATIVE_ASYNC_ACTIONCALLBACKERROR_IPP__ */
