#include "native/worker/WorkerCallback.hpp"

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

namespace native {

WorkerCallbackBase::WorkerCallbackBase(std::shared_ptr<Loop> iLoop) :
        WorkerBase(iLoop) {
}

std::shared_ptr<WorkerCallbackBase> WorkerCallbackBase::getInstance() {
    return std::static_pointer_cast<WorkerCallbackBase, WorkerBase>(WorkerBase::getInstance());
}

void WorkerCallbackBase::executeWorker() {
    NNATIVE_FCALL();
    this->resolveCb();
}

void WorkerCallbackBase::resolve() {
    NNATIVE_FCALL();
    this->enqueue();
}

} /* namespace native */
