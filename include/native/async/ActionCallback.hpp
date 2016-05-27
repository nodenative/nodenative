#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_HPP__
#define __NATIVE_ASYNC_ACTIONCALLBACK_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.hpp"
#include "ActionCallbackBase.hpp"

namespace native {

template <class R> class FutureShared;

template <typename R> class Future;

// Used by Future<void>::then()

template <typename R, typename... Args> class ActionCallback : public ActionCallbackBase<void> {
  std::function<R(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;
  ActionCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  static std::shared_ptr<ActionCallback<R, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

  ActionCallback() = delete;

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename R, typename... Args> class ActionCallback<Future<R>, Args...> : public ActionCallbackBase<void> {
  std::function<Future<R>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;
  ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  static std::shared_ptr<ActionCallback<Future<R>, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

  ActionCallback() = delete;

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename... Args> class ActionCallback<Future<void>, Args...> : public ActionCallbackBase<void> {
  std::function<Future<void>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;

  ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallback() = delete;
  static std::shared_ptr<ActionCallback<Future<void>, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

template <typename... Args> class ActionCallback<void, Args...> : public ActionCallbackBase<void> {
  std::function<void(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;

  ActionCallback(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallback() = delete;
  static std::shared_ptr<ActionCallback<void, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

// Used by Future<R>::then()

/** Value resolver callback class template. It call the function callback in case if the future resolved
 */
template <typename R, typename P, typename... Args> class ActionCallbackP1 : public ActionCallbackBase<P> {
  std::function<R(P, Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(P p, helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  ActionCallbackP1() = delete;
  static std::shared_ptr<ActionCallbackP1<R, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename R, typename P, typename... Args>
class ActionCallbackP1<Future<R>, P, Args...> : public ActionCallbackBase<P> {
  std::function<Future<R>(P, Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(P p, helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  ActionCallbackP1() = delete;
  static std::shared_ptr<ActionCallbackP1<Future<R>, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename P, typename... Args>
class ActionCallbackP1<Future<void>, P, Args...> : public ActionCallbackBase<P> {
  std::function<Future<void>(P, Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(P p, helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallbackP1() = delete;
  static std::shared_ptr<ActionCallbackP1<Future<void>, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

template <typename P, typename... Args> class ActionCallbackP1<void, P, Args...> : public ActionCallbackBase<P> {
  std::function<void(P, Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(P p, helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallbackP1() = delete;
  static std::shared_ptr<ActionCallbackP1<void, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACK_HPP__ */
