#ifndef __NATIVE_ASYNC_ACTIONCALLBACKFINALLY_HPP__
#define __NATIVE_ASYNC_ACTIONCALLBACKFINALLY_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.hpp"
#include "ActionCallbackBase.hpp"
#include "AsyncBase.hpp"
#include "FutureError.hpp"
#include "FutureSharedResolver.hpp"

namespace native {

template <class R> class FutureShared;

template <typename R> class Future;

// Used by Future<void>::finally()

template <typename R, typename... Args> class ActionCallbackFinally : public ActionCallbackBase<void> {
  std::function<R(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;
  ActionCallbackFinally(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  static std::shared_ptr<ActionCallbackFinally<R, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

  ActionCallbackFinally() = delete;

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename R, typename... Args>
class ActionCallbackFinally<Future<R>, Args...> : public ActionCallbackBase<void> {
  std::function<Future<R>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;
  ActionCallbackFinally(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  static std::shared_ptr<ActionCallbackFinally<Future<R>, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

  ActionCallbackFinally() = delete;

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename... Args> class ActionCallbackFinally<Future<void>, Args...> : public ActionCallbackBase<void> {
  std::function<Future<void>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackFinally(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallbackFinally() = delete;
  static std::shared_ptr<ActionCallbackFinally<Future<void>, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

template <typename... Args> class ActionCallbackFinally<void, Args...> : public ActionCallbackBase<void> {
  std::function<void(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackFinally(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallbackFinally() = delete;
  static std::shared_ptr<ActionCallbackFinally<void, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

// Used by Future<R>::finally()

/** Value resolver callback class template. It call the function callback in case if the future resolved
 */
template <typename R, typename P, typename... Args> class ActionCallbackFinallyP1 : public ActionCallbackBase<P> {
  std::function<R(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  ActionCallbackFinallyP1() = delete;
  static std::shared_ptr<ActionCallbackFinallyP1<R, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename R, typename P, typename... Args>
class ActionCallbackFinallyP1<Future<R>, P, Args...> : public ActionCallbackBase<P> {
  std::function<Future<R>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  ActionCallbackFinallyP1() = delete;
  static std::shared_ptr<ActionCallbackFinallyP1<Future<R>, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
};

template <typename P, typename... Args>
class ActionCallbackFinallyP1<Future<void>, P, Args...> : public ActionCallbackBase<P> {
  std::function<Future<void>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallbackFinallyP1() = delete;
  static std::shared_ptr<ActionCallbackFinallyP1<Future<void>, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

template <typename P, typename... Args> class ActionCallbackFinallyP1<void, P, Args...> : public ActionCallbackBase<P> {
  std::function<void(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb(P p) override;
  void rejectCb(const FutureError &iError) override;

  ActionCallbackFinallyP1(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

public:
  typedef void ResultType;

  ActionCallbackFinallyP1() = delete;
  static std::shared_ptr<ActionCallbackFinallyP1<void, P, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACKFINALLY_HPP__ */
