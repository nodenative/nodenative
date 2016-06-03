#ifndef __NATIVE_WORKER_HPP__
#define __NATIVE_WORKER_HPP__

#include <uv.h>

#include "async/Future.hpp"
#include "worker/WorkerBase.hpp"
#include "worker/WorkerCallback.hpp"

#include <type_traits>

namespace native {

/** Enqueue a thread/parallel task callback. After the completion. The returned value will be enqueued in the current
 * event Loop.
 *
 * The return callback may accept value or reference parameters
 * The function callback may return a void value, copied value, reference value or Future value.
 * @param f a function object callback to be enqueued
 * @Params Args... arguments of the function object
 * @return future object of the callback return, to access the value it can be only accessed via .then method of the
 * function object
 * @note The function object may be copied, internally, the shared area will not be copied.
 */
template <class F, class... Args>
Future<typename WorkerCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>
worker(F &&f, Args &&... args) {
  NNATIVE_FCALL();
  std::shared_ptr<Loop> iLoop = Loop::GetInstance();
  return worker<F, Args...>(iLoop, std::forward<F>(f), std::forward<Args>(args)...);
}

/** Enqueue a thread/parallel task callback. After the completion. The returned value will be enqueued in the specified
 * event Loop.
 *
 * The return callback may accept value or reference parameters
 * The function callback may return a void value, copied value, reference value or Future value.
 * @param iLoop the Loop queue
 * @param f a function object callback to be enqueued
 * @Params Args... arguments of the function object
 * @return future object of the callback return, to access the value it can be only accessed via .then method of the
 * function object
 * @note The function object may be copied, internally, the shared area will not be copied.
 */
template <class F, class... Args>
Future<typename WorkerCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>
worker(std::shared_ptr<Loop> iLoop, F &&f, Args &&... args) {
  NNATIVE_FCALL();
  using return_type = typename std::result_of<F(Args...)>::type;
  std::shared_ptr<WorkerCallback<return_type, Args...>> action =
      WorkerCallback<return_type, Args...>::Create(iLoop, std::forward<F>(f), std::forward<Args>(args)...);
  auto future = Future<typename WorkerCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>(
      action->getFuture());
  action->resolve();
  return future;
}

} /* namespace native */

#endif /* __NATIVE_WORKER_HPP__ */
