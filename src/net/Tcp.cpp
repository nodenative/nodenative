#include "native/net/Tcp.hpp"
#include "native/async.hpp"
#include "native/async.ipp"
#include "native/async/RequestPromise.hpp"
#include "native/net/net_utils.hpp"

namespace native {
namespace net {

std::shared_ptr<Tcp> Tcp::Create() {
  std::shared_ptr<Tcp> instance(new Tcp(Loop::GetInstanceOrCreateDefault()));
  instance->init();
  return instance;
}

std::shared_ptr<Tcp> Tcp::Create(std::shared_ptr<native::Loop> iLoop) {
  std::shared_ptr<Tcp> instance(new Tcp(iLoop));
  instance->init();
  return instance;
}

Tcp::Tcp(std::shared_ptr<native::Loop> iLoop)
    : base::Stream(iLoop), _uvTcpInstance(new uv_tcp_t), _connecting(false), _connected(false) {
  NNATIVE_FCALL();
}

Tcp::~Tcp() { NNATIVE_FCALL(); }

void Tcp::init() {
  init(reinterpret_cast<uv_handle_t *>(_uvTcpInstance.get()));
  uv_tcp_init(_loop->get(), get<uv_tcp_t>());
}

void Tcp::init(uv_handle_t *iHandlePtr) { Stream::init(iHandlePtr); }

bool Tcp::bind(const std::string &ip, const int port, Error &oError, const int version) {
  std::unique_ptr<sockaddr> addrInstance;

  if (version == 4) {
    ip4_addr *addrPtr = new ip4_addr;
    to_ip4_addr(ip, port, *addrPtr, oError);
    addrInstance.reset(reinterpret_cast<sockaddr *>(addrPtr));
  } else if (version == 6) {
    ip6_addr *addrPtr = new ip6_addr;
    to_ip6_addr(ip, port, *addrPtr, oError);
    addrInstance.reset(reinterpret_cast<sockaddr *>(addrPtr));
  } else {
    NNATIVE_DEBUG("wrong address version " << version << ". Accept only 4 or 6")
    return false;
  }

  if (oError) {
    NNATIVE_DEBUG("Error in ipX_to_addr: " << oError.str());
    return false;
  }

  oError = uv_tcp_bind(get<uv_tcp_t>(), addrInstance.get(), 0);
  NNATIVE_DEBUG("uv_tcp_bind result: " << oError.code());
  return !oError;
}

bool Tcp::bind(const std::string &ip, const int port, const int version) {
  Error err;
  return bind(ip, port, err, version);
}

void Tcp::Connect(uv_connect_t *req, int status) {
  std::unique_ptr<RequestPromiseInstance<void, uv_connect_t, Tcp>> reqInstance(
      static_cast<RequestPromiseInstance<void, uv_connect_t, Tcp> *>(req->data));
  NNATIVE_ASSERT(req->handle->data);
  reqInstance->_instance->_connected = true;
  reqInstance->_instance->_connecting = false;
  if (status == 0) {
    reqInstance->_promise.resolve();
  } else {
    PROMISE_REJECT(reqInstance->_promise, "Error in uv_tcp_connect, non-zero status " << status);
  }
}

Future<void> Tcp::connect(const std::string &ip, const int port, const int version) {
  if (_connected || _connecting) {
    return Promise<void>::Reject(_loop, "Already connected");
  }

  Error currError;
  std::unique_ptr<sockaddr> addrInstance;

  if (version == 4) {
    ip4_addr *addrPtr = new ip4_addr;
    to_ip4_addr(ip, port, *addrPtr, currError);
    addrInstance.reset(reinterpret_cast<sockaddr *>(addrPtr));
  } else if (version == 6) {
    ip6_addr *addrPtr = new ip6_addr;
    to_ip6_addr(ip, port, *addrPtr, currError);
    addrInstance.reset(reinterpret_cast<sockaddr *>(addrPtr));
  } else {
    return Promise<void>::Reject(_loop, "wrong socket address");
  }

  if (currError) {
    return Promise<void>::Reject(_loop, "Error in to_ip4_addr/to_ip6_addr");
  }

  std::unique_ptr<RequestPromiseInstance<void, uv_connect_t, Tcp>> reqInstance(
      new RequestPromiseInstance<void, uv_connect_t, Tcp>(_loop, getInstanceTcp()));
  currError = uv_tcp_connect(&reqInstance->_req, get<uv_tcp_t>(), addrInstance.get(), &Tcp::Connect);

  if (currError) {
    return Promise<void>::Reject(_loop, "Error in uv_tcp_connect");
  }

  Future<void> future = reqInstance->_promise.getFuture();

  reqInstance.release();
  _connecting = true;
  return future;
}

bool Tcp::getsockname(bool &ip4, std::string &ip, int &port) {
  struct sockaddr_storage addr;
  int len = sizeof(addr);
  if (uv_tcp_getsockname(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr *>(&addr), &len) == 0) {
    ip4 = (addr.ss_family == AF_INET);
    if (ip4)
      return from_ip4_addr(reinterpret_cast<ip4_addr *>(&addr), ip, port);
    else
      return from_ip6_addr(reinterpret_cast<ip6_addr *>(&addr), ip, port);
  }
  return false;
}

bool Tcp::getpeername(bool &ip4, std::string &ip, int &port) {
  struct sockaddr_storage addr;
  int len = sizeof(addr);
  if (uv_tcp_getpeername(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr *>(&addr), &len) == 0) {
    ip4 = (addr.ss_family == AF_INET);
    if (ip4)
      return from_ip4_addr(reinterpret_cast<ip4_addr *>(&addr), ip, port);
    else
      return from_ip6_addr(reinterpret_cast<ip6_addr *>(&addr), ip, port);
  }
  return false;
}

} /* namespace native */
} /* namespace net */
