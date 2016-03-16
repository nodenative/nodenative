#include "native/worker/WorkerBase.hpp"
#include "native/helper/trace.hpp"
#include "native/Error.hpp"

namespace native {

WorkerBase::WorkerBase(std::shared_ptr<Loop> iLoop) : _loop(iLoop) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_loop);
}

WorkerBase::~WorkerBase() {
    NNATIVE_FCALL();
    // Check if the destructor is called with pending job
    NNATIVE_ASSERT(this->_uvWork.data == nullptr);
}

void WorkerBase::enqueue() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(!_instance);
    _uvWork.data = this;

    if(uv_queue_work(_loop->get(), &_uvWork, &WorkerBase::Worker, &WorkerBase::WorkerAfter) != 0) {
        NNATIVE_DEBUG("Error in uv_queue_work");
        throw Exception("uv_queue_work");
    }
    _instance = getInstance();
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
    NNATIVE_DEBUG("iStatus: " << iStatus);

    WorkerBase* currobj = static_cast<WorkerBase*>(iHandle->data);
    iHandle->data = nullptr;

    NNATIVE_ASSERT(currobj->_instance);

    // Save a instance copy do not call the destructor
    std::shared_ptr<WorkerBase> currInst = currobj->_instance;
    currobj->_instance.reset();

    currobj->executeWorkerAfter(iStatus);
    currobj->closeWorker();
}

} /* namespace native */

