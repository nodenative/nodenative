#include "native/async/ActionCallback.hpp"
#include "native/async/Future.hpp"
#include "native/async/ActionCallback.ipp"
#include "native/helper/trace.hpp"

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

ActionCallbackBase<void>::ActionCallbackBase(std::shared_ptr<Loop> iLoop) :
        AsyncBase(iLoop) {
}

std::shared_ptr<ActionCallbackBase<void>> ActionCallbackBase<void>::getInstance() {
    return shared_from_this();
}

void ActionCallbackBase<void>::executeAsync() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_resolver);
    NNATIVE_ASSERT(_instance);
    _resolver->resolveCb(_instance);
}

void ActionCallbackBase<void>::closeAsync(std::unique_ptr<AsyncBase> iInstance) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_resolver);
    NNATIVE_ASSERT(_instance);
    NNATIVE_ASSERT(iInstance.get() == this);
    iInstance.release();
    _resolver.reset();
    _instance.reset();
}

void ActionCallbackBase<void>::resolve() {
    NNATIVE_ASSERT(!_instance);
    NNATIVE_ASSERT(!_resolver);
    _instance = getInstance();
    _resolver = std::make_unique<FutureSharedResolverValue<void>>();
    enqueue();
}

void ActionCallbackBase<void>::reject(const FutureError &iError) {
    NNATIVE_ASSERT(!_instance);
    NNATIVE_ASSERT(!_resolver);
    _instance = getInstance();
    _resolver = std::make_unique<FutureSharedResolverError<void>>(iError);
    enqueue();
}

} /* namespace native */

