#ifndef __NATIVE_WORKER_WORKERCALLBACK_HPP__
#define __NATIVE_WORKER_WORKERCALLBACK_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../async/FutureError.hpp"
#include "../async/FutureSharedResolver.hpp"
#include "../helper/TemplateSeqInd.hpp"
#include "WorkerBase.hpp"

namespace native {

template <class R> class FutureShared;

template <typename R> class Future;

/** Acction callback base class template specialization for void type.
 */
class WorkerCallbackBase : public WorkerBase {
protected:
  WorkerCallbackBase(std::shared_ptr<Loop> iLoop);

  void executeWorker() override;

public:
  virtual ~WorkerCallbackBase() {}
  std::shared_ptr<WorkerCallbackBase> getInstance();
  void resolve();

  virtual void resolveCb() = 0;
};

template <typename R, typename... Args> class WorkerCallback : public WorkerCallbackBase {
  std::function<R(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;
  std::unique_ptr<FutureSharedResolver<R>> _resolver;

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

public:
  typedef R ResultType;

  WorkerCallback() = delete;
  static std::shared_ptr<WorkerCallback<R, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
  void executeWorkerAfter(int iStatus) override;
  std::shared_ptr<WorkerCallback<R, Args...>> getInstance();
};

template <typename R, typename... Args> class WorkerCallback<Future<R>, Args...> : public WorkerCallbackBase {
  WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void executeWorkerAfter(int iStatus) override;

  std::function<Future<R>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<R>> _future;
  std::unique_ptr<FutureSharedResolver<R>> _resolver;

public:
  typedef R ResultType;

  WorkerCallback() = delete;
  static std::shared_ptr<WorkerCallback<Future<R>, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<R>> getFuture();
  std::shared_ptr<WorkerCallback<Future<R>, Args...>> getInstance();
};

template <typename... Args> class WorkerCallback<Future<void>, Args...> : public WorkerCallbackBase {
  WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void executeWorkerAfter(int iStatus) override;

  std::function<Future<void>(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;
  std::unique_ptr<FutureSharedResolver<void>> _resolver;

public:
  typedef void ResultType;

  WorkerCallback() = delete;
  static std::shared_ptr<WorkerCallback<Future<void>, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
  std::shared_ptr<WorkerCallback<Future<void>, Args...>> getInstance();
};

template <typename... Args> class WorkerCallback<void, Args...> : public WorkerCallbackBase {
  WorkerCallback(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

  template <std::size_t... Is> void callFn(helper::TemplateSeqInd<Is...>);

  void resolveCb() override;
  void executeWorkerAfter(int iStatus) override;

  std::function<void(Args...)> _f;
  std::tuple<Args...> _args;
  std::shared_ptr<FutureShared<void>> _future;
  std::unique_ptr<FutureSharedResolver<void>> _resolver;

public:
  typedef void ResultType;

  WorkerCallback() = delete;
  static std::shared_ptr<WorkerCallback<void, Args...>>
  Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args &&... args);

  std::shared_ptr<FutureShared<void>> getFuture();
  std::shared_ptr<WorkerCallback<void, Args...>> getInstance();
};

} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERCALLBACK_HPP__ */
