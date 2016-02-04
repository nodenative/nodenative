#ifndef __NATIVE_ASYNC_H__
#define __NATIVE_ASYNC_H__

#include "async/Promise.h"
#include "async/AsyncCallback.h"
#include "helper/trace.h"
#include <uv.h>

#include <type_traits>

namespace native {

/** Enqueue in the default loop a function callback.
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
Future<typename std::result_of<F(Args...)>::type>
async(F&& f, Args&&... args) {
    NNATIVE_FCALL();
    loop default_loop(true);
    return async<F, Args...>(default_loop, std::forward<F>(f), std::forward<Args>(args)...);
}

/** Enqueue in the loop a function callback.
 *
 * The return callback may accept value or reference parameters
 * The function callback may return a void value, copied value or reference value.
 * @param iLoop the loop queue
 * @param f a function object callback to be enqueued
 * @Params Args... arguments of the function object
 * @return future object of the callback return, to access the value it can be only accessed via .then method of the function object
 * @note The function object may be copied, internally, the shared area will not be copied.
 */
template<class F, class... Args>
Future<typename std::result_of<F(Args...)>::type>
async(loop &iLoop, F&& f, Args&&... args) {
    NNATIVE_FCALL();
    return AsyncCallback<typename std::result_of<F(Args...)>::type, Args...>::Create(iLoop, std::forward<F>(f), std::forward<Args>(args)...);
}

} // namespace native

#endif // __NATIVE_ASYNC_H__
