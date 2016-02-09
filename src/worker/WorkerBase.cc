#include "native/worker/WorkerBase.h"
#include "native/helper/trace.h"

namespace native {

WorkerBase::WorkerBase(loop &iLoop) {
    NNATIVE_FCALL();
    _loop = iLoop.getShared();
}

WorkerBase::WorkerBase(std::shared_ptr<uv_loop_t> iLoop) {
    NNATIVE_FCALL();
    _loop = iLoop;
}

WorkerBase::~WorkerBase() {
    NNATIVE_FCALL();
    // Check if the destructor is called with pending job
    NNATIVE_ASSERT(this->_uvWork.data == nullptr);
}

void WorkerBase::enqueue() {
    NNATIVE_FCALL();
    _uvWork.data = this;
    NNATIVE_ASSERT(_loop);

    if(uv_queue_work(_loop.get(), &_uvWork, &WorkerBase::Worker, &WorkerBase::WorkerAfter) != 0) {
        NNATIVE_DEBUG("Error in uv_queue_work");
        throw native::exception("uv_queue_work");
    }
    NNATIVE_DEBUG("Enqueued");
}

void WorkerBase::Worker(uv_work_t* iHandle) {
    NNATIVE_FCALL();

    NNATIVE_ASSERT(iHandle->data != nullptr);
    WorkerBase* currobj = static_cast<WorkerBase*>(iHandle->data);

    currobj->executeWorker();
}

void WorkerBase::WorkerAfter(uv_work_t* iHandle, int iStatus) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iHandle->data != nullptr);
    WorkerBase* currobj = static_cast<WorkerBase*>(iHandle->data);
    iHandle->data = nullptr;

    // Move the pointer to a temporary variable.
    std::unique_ptr<WorkerBase> currInst(currobj);
    currobj->executeWorkerAfter(iStatus);
    currobj->closeWorker(std::move(currInst));
}
} /* namespace native */

