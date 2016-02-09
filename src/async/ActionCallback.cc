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

void ActionCallbackBase<void>::SetValue(std::shared_ptr<ActionCallbackBase<void>> iInstance) {
    NNATIVE_ASSERT(iInstance);
    iInstance->_instance = iInstance;
    ActionCallbackBaseDetached<void>::Enqueue(iInstance);
}

void ActionCallbackBase<void>::SetError(std::shared_ptr<ActionCallbackBase<void>> iInstance, const FutureError &iError) {
    NNATIVE_ASSERT(iInstance);
    iInstance->_instance = iInstance;
    ActionCallbackBaseDetachedError<void>::Enqueue(iInstance, iError);
}

} /* namespace native */

