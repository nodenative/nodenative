#ifndef __NATIVE_ASYNC_ACTIONCALLBACKERROR_HPP__
#define __NATIVE_ASYNC_ACTIONCALLBACKERROR_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.hpp"
#include "ActionCallbackBase.hpp"

namespace native {

template <class R> class FutureShared;

template <typename R> class Future;

template <typename... Args> class ActionCallbackError : public ActionCallbackBase<void> {
  std::function<void(const FutureError &, Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(const FutureError &iError, helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackError(std::shared_ptr<Loop> iLoop,
                      std::function<void(const FutureError &, Args...)> f,
                      Args &&... args);

public:
  ActionCallbackError() = delete;
  static std::shared_ptr<ActionCallbackError<Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<void(const FutureError &, Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

/** Value resolver callback class template. It call the function callback in case if the future resolved
 */
template <typename R, typename... Args> class ActionCallbackErrorP1 : public ActionCallbackBase<R> {
  std::function<R(const FutureError &, Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(const FutureError &iError, helper::TemplateSeqInd<Is...>);

  void resolveCb(R r) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackErrorP1(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError &, Args...)> f, Args &&... args);

public:
  ActionCallbackErrorP1() = delete;
  static std::shared_ptr<ActionCallbackErrorP1<R, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError &, Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACKERROR_HPP__ */
