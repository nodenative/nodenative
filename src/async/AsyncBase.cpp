#include "native/async/AsyncBase.hpp"
#include "native/Error.hpp"
#include "native/helper/trace.hpp"

#include <mutex>

namespace {
// TODO: get rid of mutexes
std::mutex mutexInProcess;
std::mutex mutexInstanceChange;
}

namespace native {

AsyncBase::AsyncBase(std::shared_ptr<Loop> iLoop) : _loop(iLoop), _instanceNum(0) {
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
  std::lock_guard<std::mutex> guard(mutexInProcess);

  // TODO: avoid race condition. This may be called from multiple threads
  NNATIVE_ASSERT(!this->_instance);
  // Make sure that the the async is not in progress
  NNATIVE_ASSERT(this->_uv_async.data == nullptr);
  // Save the instance do prevent destructor
  increaseInstance();
  this->_uv_async.data = this;

  if (uv_async_init(_loop->get(), &_uv_async, AsyncBase::Async) != 0) {
    NNATIVE_DEBUG("Error in uv_async_init");
    throw Exception("uv_async_init");
  }

  if (uv_async_send(&_uv_async) != 0) {
    NNATIVE_DEBUG("Error in uv_async_send");
    _uv_async.data = nullptr;
    this->_instance.reset();
    throw Exception("uv_async_send");
  }
  // NNATIVE_INFO("Enqueued");
}

void AsyncBase::Async(uv_async_t *iHandle) {
  NNATIVE_FCALL();
  // NNATIVE_INFO("ASYNC");

  NNATIVE_ASSERT(iHandle->data != nullptr);
  AsyncBase *currobj = static_cast<AsyncBase *>(iHandle->data);

  currobj->executeAsync();

  uv_close((uv_handle_t *)(iHandle), AsyncBase::AsyncClosed);
}

void AsyncBase::increaseInstance() {
  std::lock_guard<std::mutex> guard(mutexInstanceChange);
  ++_instanceNum;
  this->_instance = this->getInstance();
}

void AsyncBase::decreaseInstance() {
  std::lock_guard<std::mutex> guard(mutexInstanceChange);
  if (_instanceNum == 0) {
    throw Exception("decreaseInstance too many times");
  }

  if (--_instanceNum == 0) {
    _instance.reset();
  }
}

void AsyncBase::AsyncClosed(uv_handle_t *iHandle) {
  NNATIVE_FCALL();
  std::shared_ptr<AsyncBase> currInst;
  {
    std::lock_guard<std::mutex> guard(mutexInProcess);
    NNATIVE_ASSERT(iHandle->data != nullptr);

    AsyncBase *currobj = static_cast<AsyncBase *>(iHandle->data);
    iHandle->data = nullptr;

    NNATIVE_ASSERT(currobj->_instance);

    // Save a instance copy do not call the destructor
    currInst = currobj->_instance;
    currInst->decreaseInstance();
  }

  currInst->closeAsync();
}

} /* namespace native */
