#include "native/async/AsyncBase.h"
#include "native/helper/ContainerHelper.h"
#include "native/helper/ContainerHelper.i"
#include "native/helper/trace.h"

namespace native {

AsyncBase::AsyncBase(loop &iLoop) {
    _loop = iLoop._uv_loop;
}

void AsyncBase::enqueue() {
    NNATIVE_FCALL();
    _uv_async.data = this;
    uv_loop_t* currLoop = nullptr;

    if(_loop) {
        NNATIVE_DEBUG("get defined loop");
        currLoop = _loop.get();
    } else {
        NNATIVE_DEBUG("get default loop");
        currLoop = uv_default_loop();
    }

    if(uv_async_init(currLoop, &_uv_async, &AsyncBase::Async) != 0) {
        NNATIVE_DEBUG("Error in uv_async_init");
        throw native::exception("uv_async_init");
    }

    if(uv_async_send(&_uv_async) != 0) {
        NNATIVE_DEBUG("Error in uv_async_send");
        throw native::exception("uv_async_send");
    }
    NNATIVE_DEBUG("Enqueued");
}

void AsyncBase::Async(uv_async_t* iAsync) {
    NNATIVE_FCALL();

    //AsyncBase* currobj = helper::Containerof(AsyncBase::_uv_async, iAsync);
    AsyncBase* currobj = static_cast<AsyncBase*>(iAsync->data);

    uv_close((uv_handle_t*)&currobj->_uv_async, NULL);

    // Move the shared pointer to a temporary variable.
    std::shared_ptr<AsyncBase> currInst(currobj);

    currInst->executeAsync(currInst);
}

} /* namespace native */
