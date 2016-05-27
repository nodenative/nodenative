#ifndef __NATIVE_ASYNC_FEATURESHARED_HPP__
#define __NATIVE_ASYNC_FEATURESHARED_HPP__

#include "../Loop.hpp"
#include "./FutureSharedResolver.hpp"
#include "ActionCallback.hpp"
#include "ActionCallbackError.hpp"
#include "ActionCallbackFinally.hpp"

#include <atomic>
#include <memory>
#include <vector>

namespace native {

template <class R> class FutureShared : public std::enable_shared_from_this<FutureShared<R>> {
private:
  std::shared_ptr<Loop> _loop;
  std::unique_ptr<FutureSharedResolver<R>> _resolver;
  std::vector<std::shared_ptr<ActionCallbackBase<R>>> _actions;

  FutureShared(std::shared_ptr<Loop> iLoop) : _loop(iLoop) {}

public:
  typedef R result_type;

  FutureShared() = delete;
  FutureShared(const FutureShared<R> &) = delete;

  static std::shared_ptr<FutureShared<R>> Create(std::shared_ptr<Loop> iLoop);

  void resolve(R iVal);
  void reject(const FutureError &iError);

  std::shared_ptr<Loop> getLoop() const { return _loop; }

  template <class F, typename... Args>
  std::shared_ptr<
      FutureShared<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>>
  then(F &&f, Args &&... args);

  template <class F, typename... Args> std::shared_ptr<FutureShared<R>> error(F &&f, Args &&... args);

  template <class F, typename... Args>
  std::shared_ptr<
      FutureShared<typename ActionCallbackFinallyP1<typename std::result_of<F(Args...)>::type, R, Args...>::ResultType>>
  finally(F &&f, Args &&... args);
};

template <> class FutureShared<void> : public std::enable_shared_from_this<FutureShared<void>> {
private:
  std::shared_ptr<Loop> _loop;
  std::unique_ptr<FutureSharedResolver<void>> _resolver;
  std::vector<std::shared_ptr<ActionCallbackBase<void>>> _actions;

  FutureShared(std::shared_ptr<Loop> iLoop) : _loop(iLoop) {}

public:
  typedef void result_type;

  FutureShared() = delete;
  FutureShared(const FutureShared<void> &) = delete;

  static std::shared_ptr<FutureShared<void>> Create(std::shared_ptr<Loop> iLoop);

  void resolve();
  void reject(const FutureError &iError);

  std::shared_ptr<Loop> getLoop() const { return _loop; }

  template <class F, typename... Args>
  std::shared_ptr<FutureShared<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
  then(F &&f, Args &&... args);

  template <class F, typename... Args> std::shared_ptr<FutureShared<void>> error(F &&f, Args &&... args);

  template <class F, typename... Args>
  std::shared_ptr<
      FutureShared<typename ActionCallbackFinally<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
  finally(F &&f, Args &&... args);
};

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_HPP__
