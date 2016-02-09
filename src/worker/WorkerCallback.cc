#include "native/worker/WorkerCallback.h"

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

WorkerCallbackBaseDetached::WorkerCallbackBaseDetached(std::shared_ptr<WorkerCallbackBase> iInstance) :
        WorkerBase(iInstance->getLoop()),
        _instance(iInstance) {
}

void WorkerCallbackBaseDetached::executeWorker() {
    NNATIVE_FCALL();
    _instance->setValueCb();
}

void WorkerCallbackBaseDetached::executeWorkerAfter(int iStatus) {
    NNATIVE_FCALL();
    // TODO: Handle the status
    NNATIVE_ASSERT(iStatus==0);
}

void WorkerCallbackBaseDetached::Enqueue(std::shared_ptr<WorkerCallbackBase> iInstance) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iInstance);
    std::unique_ptr<WorkerCallbackBaseDetached> detachedInst(new WorkerCallbackBaseDetached(iInstance));
    detachedInst->enqueue();
    detachedInst.release();
}

void WorkerCallbackBase::SetValue(std::shared_ptr<WorkerCallbackBase> iInstance) {
    NNATIVE_ASSERT(iInstance);
    iInstance->_instance = iInstance;
    WorkerCallbackBaseDetached::Enqueue(iInstance);
}

} /* namespace native */
