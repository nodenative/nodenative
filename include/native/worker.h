#ifndef __NATIVE_WORKER_H__
#define __NATIVE_WORKER_H__

#include <uv.h>

#include "worker/WorkerBase.h"
#include "worker/WorkerCallback.h"
#include "async/Future.h"

#include <type_traits>

namespace native {

/** Enqueue a thread/parallel task callback with the default task/event Loop.
 *
 * The return callback may accept value or reference parameters
 * The function callback may return a void value, copied value or reference value.
 * @param f a function object callback to be enqueued
 * @Params Args... arguments of the function object
 * @return future object of the callback return, to access the value it can be only accessed via .then method of the function object
 * @note The function object may be copied, internally, the shared area will not be copied.
 *
 */
template<class F, class... Args>
Future<typename WorkerCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>
worker(F&& f, Args&&... args) {
    NNATIVE_FCALL();
    Loop default_loop(true);
    return worker<F, Args...>(default_loop, std::forward<F>(f), std::forward<Args>(args)...);
}

/** Enqueue a thread/parallel task callback. After the completion. The returned value will be enqueued in the specified event Loop.
 *
 * The return callback may accept value or reference parameters
 * The function callback may return a void value, copied value, reference value or Future value.
 * @param iLoop the Loop queue
 * @param f a function object callback to be enqueued
 * @Params Args... arguments of the function object
 * @return future object of the callback return, to access the value it can be only accessed via .then method of the function object
 * @note The function object may be copied, internally, the shared area will not be copied.
 */
template<class F, class... Args>
Future<typename WorkerCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>
worker(Loop &iLoop, F&& f, Args&&... args) {
    NNATIVE_FCALL();
    using return_type = typename std::result_of<F(Args...)>::type;
    std::shared_ptr<WorkerCallback<return_type, Args...>> action(new WorkerCallback<return_type, Args...>(iLoop.getShared(), std::forward<F>(f), std::forward<Args>(args)...));
    action->SetValue(action);
    return Future<typename WorkerCallback<typename std::result_of<F(Args...)>::type, Args...>::ResultType>(action->getFuture());
}

} /* namespace native */

#endif /* __NATIVE_WORKER_H__ */
