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
    return std::static_pointer_cast<ActionCallbackBase<void>, AsyncBase>(AsyncBase::getInstance());
}

void ActionCallbackBase<void>::executeAsync() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_resolver);
    _resolver->resolveCb(this->getInstance());
}

void ActionCallbackBase<void>::closeAsync() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_resolver);
    _resolver.reset();
}

void ActionCallbackBase<void>::resolve() {
    NNATIVE_ASSERT(!_resolver);
    _resolver = std::make_unique<FutureSharedResolverValue<void>>();
    enqueue();
}

void ActionCallbackBase<void>::reject(const FutureError &iError) {
    NNATIVE_ASSERT(!_resolver);
    _resolver = std::make_unique<FutureSharedResolverError<void>>(iError);
    enqueue();
}

} /* namespace native */

