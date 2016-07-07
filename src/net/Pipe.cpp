#include "native/net/Pipe.hpp"
#include "native/async.hpp"
#include "native/async.ipp"
#include "native/async/RequestPromise.hpp"
#include "native/net/net_utils.hpp"

namespace native {
namespace net {

std::shared_ptr<Pipe> Pipe::Create(const bool ipc) {
  std::shared_ptr<Pipe> instance(new Pipe(Loop::GetInstanceSafe(), ipc));
  instance->init();
  return instance;
}

std::shared_ptr<Pipe> Pipe::Create(std::shared_ptr<native::Loop> iLoop, const bool ipc) {
  std::shared_ptr<Pipe> instance(new Pipe(iLoop, ipc));
  instance->init();
  return instance;
}

Pipe::Pipe(std::shared_ptr<native::Loop> iLoop, const bool ipc)
    : base::Stream(iLoop), _uvPipeInstance(new uv_pipe_t), _ipc(ipc), _connecting(false), _connected(false) {
  NNATIVE_FCALL();
}

Pipe::~Pipe() {}

void Pipe::init() {
  init(reinterpret_cast<uv_handle_t *>(_uvPipeInstance.get()));
  uv_pipe_init(_loop->get(), get<uv_pipe_t>(), _ipc);
}

void Pipe::init(uv_handle_t *iHandlePtr) { Stream::init(iHandlePtr); }

bool Pipe::bind(const std::string &name, Error &oError) {
  std::unique_ptr<sockaddr> addrInstance;

  oError = uv_pipe_bind(get<uv_pipe_t>(), name.c_str());
  NNATIVE_DEBUG("uv_pipe_bind result: " << oError.code());
  return !oError;
}

bool Pipe::bind(const std::string &name) {
  Error err;
  return bind(name, err);
}

void Pipe::Connect(uv_connect_t *req, int status) {
  std::unique_ptr<RequestPromiseInstance<void, uv_connect_t, Pipe>> reqInstance(
      static_cast<RequestPromiseInstance<void, uv_connect_t, Pipe> *>(req->data));
  NNATIVE_ASSERT(req->handle->data);
  reqInstance->_instance->_connected = true;
  reqInstance->_instance->_connecting = false;
  if (status == 0) {
    reqInstance->_promise.resolve();
  } else {
    PROMISE_REJECT(reqInstance->_promise, "Error in uv_tcp_connect, non-zero status " << status);
  }
}

Future<void> Pipe::connect(const std::string &name) {
  if (_connected || _connecting) {
    return Promise<void>::Reject(_loop, "Already connected");
  }

  std::unique_ptr<sockaddr> addrInstance;

  std::unique_ptr<RequestPromiseInstance<void, uv_connect_t, Pipe>> reqInstance(
      new RequestPromiseInstance<void, uv_connect_t, Pipe>(_loop, getInstancePipe()));
  uv_pipe_connect(&reqInstance->_req, get<uv_pipe_t>(), name.c_str(), &Pipe::Connect);

  Future<void> future = reqInstance->_promise.getFuture();

  reqInstance.release();
  _connecting = true;
  return future;
}

bool Pipe::getsockname(std::string &name) {
  size_t len = 0;
  uv_pipe_getsockname(get<uv_pipe_t>(), nullptr, &len);
  name.resize(len);

  if (uv_pipe_getsockname(get<uv_pipe_t>(), const_cast<char *>(name.c_str()), &len) == 0) {
    return true;
  }

  return false;
}

bool Pipe::getpeername(std::string &name) {
  size_t len = 0;
  uv_pipe_getpeername(get<uv_pipe_t>(), nullptr, &len);
  name.resize(len);

  if (uv_pipe_getpeername(get<uv_pipe_t>(), const_cast<char *>(name.c_str()), &len) == 0) {
    return true;
  }

  return false;
}

int Pipe::pendingCount() { return uv_pipe_pending_count(get<uv_pipe_t>()); }

int Pipe::pendingType() { return uv_pipe_pending_type(get<uv_pipe_t>()); }

bool Pipe::setPendingInstances(int instances) {
#ifdef _WIN32
  uv_pipe_pending_instances(get<uv_pipe_t>(), instances);
  return true;
#else
  return false;
#endif
}

} /* namespace native */
} /* namespace net */
