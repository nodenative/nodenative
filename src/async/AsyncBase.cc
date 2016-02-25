#include "native/async/AsyncBase.h"
#include "native/helper/trace.h"

namespace native {

AsyncBase::AsyncBase(std::shared_ptr<Loop> iLoop) : _loop(iLoop) {
    NNATIVE_FCALL();
}

AsyncBase::~AsyncBase() {
    NNATIVE_FCALL();
    // Check if the destructor is called with pending job
    NNATIVE_ASSERT(this->_uv_async.data == nullptr);
}

void AsyncBase::enqueue() {
    NNATIVE_FCALL();
    _uv_async.data = this;
    NNATIVE_ASSERT(_loop);

    if(uv_async_init(_loop->get(), &_uv_async, &AsyncBase::Async) != 0) {
        NNATIVE_DEBUG("Error in uv_async_init");
        throw native::exception("uv_async_init");
    }

    if(uv_async_send(&_uv_async) != 0) {
        NNATIVE_DEBUG("Error in uv_async_send");
        throw native::exception("uv_async_send");
    }
    NNATIVE_DEBUG("Enqueued");
}

void AsyncBase::Async(uv_async_t* iHandle) {
    NNATIVE_FCALL();

    NNATIVE_ASSERT(iHandle->data != nullptr);
    AsyncBase* currobj = static_cast<AsyncBase*>(iHandle->data);

    uv_close((uv_handle_t*)(iHandle), &AsyncBase::AsyncClosed);

    currobj->executeAsync();
}

void AsyncBase::AsyncClosed(uv_handle_t* iHandle) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(iHandle->data != nullptr);

    AsyncBase* currobj = static_cast<AsyncBase*>(iHandle->data);
    iHandle->data = nullptr;

    // Move the pointer to a temporary variable.
    std::unique_ptr<AsyncBase> currInst(currobj);
    currobj->closeAsync(std::move(currInst));
}

} /* namespace native */
