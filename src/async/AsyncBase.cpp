#include "native/async/AsyncBase.hpp"
#include "native/helper/trace.hpp"
#include "native/Error.hpp"

namespace native {

AsyncBase::AsyncBase(std::shared_ptr<Loop> iLoop) : _loop(iLoop) {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_loop);
    _uv_async.data = nullptr;
}

AsyncBase::~AsyncBase() {
    NNATIVE_FCALL();
    // Check if the destructor is called with pending job
    NNATIVE_ASSERT(this->_uv_async.data == nullptr);
}

void AsyncBase::enqueue() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(!this->_instance);
    NNATIVE_ASSERT(this->_uv_async.data == nullptr);
    this->_uv_async.data = this;

    if(uv_async_init(_loop->get(), &_uv_async, &AsyncBase::Async) != 0) {
        NNATIVE_DEBUG("Error in uv_async_init");
        throw Exception("uv_async_init");
    }

    // Save the instance do prevent destructor
    this->_instance = this->getInstance();

    if(uv_async_send(&_uv_async) != 0) {
        NNATIVE_DEBUG("Error in uv_async_send");
        throw Exception("uv_async_send");
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

    NNATIVE_ASSERT(currobj->_instance);

    // Save a instance copy do not call the destructor
    std::shared_ptr<AsyncBase> currInst = currobj->_instance;
    currobj->_instance.reset();

    currobj->closeAsync();
}

} /* namespace native */
