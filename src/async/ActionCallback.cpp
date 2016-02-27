#include "native/async/ActionCallback.hpp"
#include "native/async/Future.hpp"
#include "native/async/ActionCallback.ipp"
#include "native/helper/trace.hpp"

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

void ActionCallbackBaseDetached<void>::executeAsync() {
    NNATIVE_FCALL();
    _instance->setValueCb();
}

void ActionCallbackBaseDetached<void>::Enqueue(std::shared_ptr<ActionCallbackBase<void>> iInstance) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iInstance);
    std::unique_ptr<ActionCallbackBaseDetached<void>> detachedInst(new ActionCallbackBaseDetached<void>(iInstance));
    detachedInst->enqueue();
    detachedInst.release();
}

void ActionCallbackBase<void>::setValue() {
    ActionCallbackBaseDetached<void>::Enqueue(this->shared_from_this());
}

void ActionCallbackBase<void>::setError(const FutureError &iError) {
    ActionCallbackBaseDetachedError<void>::Enqueue(this->shared_from_this(), iError);
}

} /* namespace native */

