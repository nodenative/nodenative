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
    NNATIVE_ASSERT(iStatus==0);
    _instance->executeWorkerAfter(iStatus);
}

void WorkerCallbackBaseDetached::Enqueue(std::shared_ptr<WorkerCallbackBase> iInstance) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iInstance);
    std::unique_ptr<WorkerCallbackBaseDetached> detachedInst(new WorkerCallbackBaseDetached(iInstance));
    detachedInst->enqueue();
    detachedInst.release();
}

void WorkerCallbackBase::setValue() {
    WorkerCallbackBaseDetached::Enqueue(this->shared_from_this());
}

} /* namespace native */
