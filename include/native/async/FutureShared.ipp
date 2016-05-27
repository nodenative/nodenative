#ifndef __NATIVE_ASYNC_FEATURESHARED_IPP__
#define __NATIVE_ASYNC_FEATURESHARED_IPP__

#include "FutureShared.hpp"

#include <memory>

namespace native {

template <class R> std::shared_ptr<FutureShared<R>> FutureShared<R>::Create(std::shared_ptr<Loop> iLoop) {
  std::shared_ptr<FutureShared<R>> instance(new FutureShared<R>(iLoop));

  return instance;
}

template <class R> void FutureShared<R>::resolve(R iVal) {
  NNATIVE_CHECK_LOOP_THREAD(this->_loop);
  if (this->_resolver) {
    throw PromiseAlreadySatisfied();
  }

  this->_resolver = std::make_unique<FutureSharedResolverValue<R>>(std::forward<R>(iVal));

  for (std::shared_ptr<ActionCallbackBase<R>> action : this->_actions) {
    this->_resolver->resolve(action);
  }
}

template <typename R> void FutureShared<R>::reject(const FutureError &iError) {
  NNATIVE_CHECK_LOOP_THREAD(this->_loop);
  if (this->_resolver) {
    throw PromiseAlreadySatisfied();
  }

  this->_resolver = std::make_unique<FutureSharedResolverError<R>>(iError);

  for (std::shared_ptr<ActionCallbackBase<R>> action : this->_actions) {
    this->_resolver->resolve(action);
  }
}

template <class R>
template <class F, typename... Args>
std::shared_ptr<
    FutureShared<typename ActionCallbackP1<typename std::result_of<F(R, Args...)>::type, R, Args...>::ResultType>>
FutureShared<R>::then(F &&f, Args &&... args) {
  using return_type = typename std::result_of<F(R, Args...)>::type;
  std::shared_ptr<ActionCallbackP1<return_type, R, Args...>> action =
      ActionCallbackP1<return_type, R, Args...>::Create(_loop, std::forward<F>(f), std::forward<Args>(args)...);
  auto currFuture = action->getFuture();

  if (!this->_loop->isNotOnEventLoopThread() && !this->_resolver) {
    _actions.push_back(action);
  } else {
    // avoid race condition
    std::shared_ptr<FutureShared<R>> instance = this->shared_from_this();
    async(this->_loop, [instance, action]() {
      instance->_actions.push_back(action);

      if (instance->_resolver) {
        instance->_resolver->resolve(action);
      }
    });
  }

  return currFuture;
}

template <class F, typename... Args>
std::shared_ptr<FutureShared<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
FutureShared<void>::then(F &&f, Args &&... args) {
  using return_type = typename std::result_of<F(Args...)>::type;
  std::shared_ptr<ActionCallback<return_type, Args...>> action =
      ActionCallback<return_type, Args...>::Create(_loop, std::forward<F>(f), std::forward<Args>(args)...);
  auto currFuture = action->getFuture();

  if (!this->_loop->isNotOnEventLoopThread() && !this->_resolver) {
    _actions.push_back(action);
  } else {
    // avoid race condition
    std::shared_ptr<FutureShared<void>> instance = this->shared_from_this();

    async(this->_loop, [instance, action]() {
      instance->_actions.push_back(action);

      if (instance->_resolver) {
        instance->_resolver->resolve(action);
      }
    });
  }
  return currFuture;
}

template <class R>
template <class F, typename... Args>
std::shared_ptr<
    FutureShared<typename ActionCallbackFinallyP1<typename std::result_of<F(Args...)>::type, R, Args...>::ResultType>>
FutureShared<R>::finally(F &&f, Args &&... args) {
  using return_type = typename std::result_of<F(Args...)>::type;
  auto action =
      ActionCallbackFinallyP1<return_type, R, Args...>::Create(_loop, std::forward<F>(f), std::forward<Args>(args)...);
  auto currFuture = action->getFuture();

  if (!this->_loop->isNotOnEventLoopThread() && !this->_resolver) {
    _actions.push_back(action);
  } else {
    // avoid race condition
    std::shared_ptr<FutureShared<R>> instance = this->shared_from_this();
    async(this->_loop, [instance, action]() {
      instance->_actions.push_back(action);

      if (instance->_resolver) {
        instance->_resolver->resolve(action);
      }
    });
  }

  return currFuture;
}

template <class F, typename... Args>
std::shared_ptr<
    FutureShared<typename ActionCallbackFinally<typename std::result_of<F(Args...)>::type, Args...>::ResultType>>
FutureShared<void>::finally(F &&f, Args &&... args) {
  using return_type = typename std::result_of<F(Args...)>::type;
  auto action =
      ActionCallbackFinally<return_type, Args...>::Create(_loop, std::forward<F>(f), std::forward<Args>(args)...);
  auto currFuture = action->getFuture();

  if (!this->_loop->isNotOnEventLoopThread() && !this->_resolver) {
    _actions.push_back(action);
  } else {
    // avoid race condition
    std::shared_ptr<FutureShared<void>> instance = this->shared_from_this();

    async(this->_loop, [instance, action]() {
      instance->_actions.push_back(action);

      if (instance->_resolver) {
        instance->_resolver->resolve(action);
      }
    });
  }
  return currFuture;
}

template <class R>
template <class F, typename... Args>
std::shared_ptr<FutureShared<R>> FutureShared<R>::error(F &&f, Args &&... args) {
  std::shared_ptr<ActionCallbackErrorP1<R, Args...>> action =
      ActionCallbackErrorP1<R, Args...>::Create(_loop, std::forward<F>(f), std::forward<Args>(args)...);
  std::shared_ptr<FutureShared<R>> currFuture = action->getFuture();

  if (!this->_loop->isNotOnEventLoopThread() && !this->_resolver) {
    _actions.push_back(action);
  } else {
    // avoid race condition
    std::shared_ptr<FutureShared<R>> instance = this->shared_from_this();

    async(this->_loop, [instance, action]() {
      instance->_actions.push_back(action);

      if (instance->_resolver) {
        instance->_resolver->resolve(action);
      }
    });
  }

  return currFuture;
}

template <class F, typename... Args>
std::shared_ptr<FutureShared<void>> FutureShared<void>::error(F &&f, Args &&... args) {
  std::shared_ptr<ActionCallbackError<Args...>> action =
      ActionCallbackError<Args...>::Create(_loop, std::forward<F>(f), std::forward<Args>(args)...);
  std::shared_ptr<FutureShared<void>> currFuture = action->getFuture();

  if (!this->_loop->isNotOnEventLoopThread() && !this->_resolver) {
    _actions.push_back(action);
  } else {
    // avoid race condition
    std::shared_ptr<FutureShared<void>> instance = this->shared_from_this();
    async(this->_loop, [instance, action]() {
      instance->_actions.push_back(action);

      if (instance->_resolver) {
        instance->_resolver->resolve(action);
      }
    });
  }

  return currFuture;
}

} /* namespace native */

#endif // __NATIVE_ASYNC_FEATURESHARED_IPP__
