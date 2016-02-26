#include "native/async/ActionCallback.h"
#include "native/async/Future.h"
#include "native/async/ActionCallback.i"
#include "native/helper/trace.h"

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

