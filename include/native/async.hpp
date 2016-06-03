#ifndef __NATIVE_ASYNC_HPP__
#define __NATIVE_ASYNC_HPP__

#include "async/ActionCallback.hpp"
#include "async/ActionCallbackBase.hpp"
#include "async/ActionCallbackError.hpp"
#include "async/ActionCallbackFinally.hpp"
#include "async/Future.hpp"
#include "async/FutureError.hpp"
#include "async/FutureShared.hpp"
#include "async/Promise.hpp"
#include "helper/trace.hpp"

#include <uv.h>

#include <type_traits>

namespace native {

/** Enqueue in the current event Loop a function callback.
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
Future<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType> async(F &&f,
                                                                                                      Args &&... args) {
  NNATIVE_FCALL();
  std::shared_ptr<Loop> currentLoop = Loop::GetInstance();

  return async<F, Args...>(currentLoop, std::forward<F>(f), std::forward<Args>(args)...);
}

/** Enqueue in the specified event Loop a function callback.
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
Future<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>
async(std::shared_ptr<Loop> iLoop, F &&f, Args &&... args) {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(iLoop);
  using return_type = typename std::result_of<F(Args...)>::type;
  std::shared_ptr<ActionCallback<return_type, Args...>> action =
      ActionCallback<return_type, Args...>::Create(iLoop, std::forward<F>(f), std::forward<Args>(args)...);
  auto future = Future<typename ActionCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>(
      action->getFuture());
  action->resolve();
  return future;
}

} // namespace native

#endif // __NATIVE_ASYNC_HPP__
