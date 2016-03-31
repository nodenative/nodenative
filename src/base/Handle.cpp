#include "native/base/Handle.hpp"
#include "native/helper/trace.hpp"

namespace native {
namespace base {

Handle::Handle(std::shared_ptr<native::Loop> iLoop) :
    _uvHandlePtr(nullptr),
    _loop(iLoop),
    _instanceHandleCount(0),
    _closingPromise(iLoop)
{
    NNATIVE_FCALL();
    NNATIVE_CHECK_LOOP_THREAD(_loop);
}

Handle::~Handle() {
    NNATIVE_FCALL();
}

std::shared_ptr<Handle> Handle::getInstanceHandle() {
    return shared_from_this();
}

bool Handle::isActive() {
    return uv_is_active(get()) != 0;
}

bool Handle::isClosing() {
    return uv_is_closing(get()) != 0;
}

void Handle::ref() {
    uv_ref(get());
}

void Handle::unref() {
    uv_unref(get());
}

Future<void> Handle::close() {
    NNATIVE_CHECK_LOOP_THREAD(_loop);
    //if(!isActive()) {
    //    return Promise<std::shared_ptr<Handle>>::Reject(_loop, "Handle is not active");
    //}

    if(!isClosing()) {
        _closingPromise = Promise<void>(_loop);
        uv_close(get(),
                 [](uv_handle_t* h) {
                     std::shared_ptr<Handle> instance = static_cast<Handle*>(h->data)->getInstanceHandle();
                     instance->releaseInstanceHandle();
                     instance->_closingPromise.resolve();
                     instance->_closingPromise.reset();
                 });
    }

    return _closingPromise.getFuture();
}

void Handle::init(uv_handle_t* iHandlePtr) {
    _uvHandlePtr = iHandlePtr;
    NNATIVE_ASSERT(_uvHandlePtr);
    _uvHandlePtr->data = this;
    keepInstanceHandle();
}

void Handle::keepInstanceHandle() {
    ++_instanceHandleCount;

    if (!_instanceHandle) {
        _instanceHandle = getInstanceHandle();
    }
}

void Handle::releaseInstanceHandle() {
    NNATIVE_ASSERT(_instanceHandleCount > 0);
    NNATIVE_ASSERT(_instanceHandle);
    if(--_instanceHandleCount == 0) {
        NNATIVE_DEBUG("reset sinatnce handle");
        _instanceHandle.reset();
    }
    NNATIVE_DEBUG("Remain handle instance count: " << _instanceHandleCount );
}

} /* namespace base */
} /* namespace native */
