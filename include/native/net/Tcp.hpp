#ifndef __NATIVE_NET_TCP_HPP__
#define __NATIVE_NET_TCP_HPP__

#include "../Loop.hpp"
#include "../base.hpp"
#include "net_utils.hpp"

namespace native {
namespace net {

class Tcp : public native::base::Stream {
public:
  static std::shared_ptr<Tcp> Create();
  static std::shared_ptr<Tcp> Create(std::shared_ptr<native::Loop> iLoop);

  Tcp() = delete;
  ~Tcp();

  bool nodelay(bool enable) { return uv_tcp_nodelay(get<uv_tcp_t>(), enable ? 1 : 0) == 0; }
  bool keepalive(bool enable, unsigned int delay) {
    return uv_tcp_keepalive(get<uv_tcp_t>(), enable ? 1 : 0, delay) == 0;
  }
  bool simultanious_accepts(bool enable) { return uv_tcp_simultaneous_accepts(get<uv_tcp_t>(), enable ? 1 : 0) == 0; }

  /** ip4 bind
   */
  bool bind(const std::string &ip, const int port, Error &oError, const int version = 4);
  bool bind(const std::string &ip, const int port, const int version = 4);

  Future<void> connect(const std::string &ip, const int port, const int version = 4);

  bool getsockname(bool &ip4, std::string &ip, int &port);
  bool getpeername(bool &ip4, std::string &ip, int &port);
  virtual void init();

  std::shared_ptr<Tcp> getInstanceTcp() {
    return std::static_pointer_cast<Tcp>(native::base::Stream::getInstanceStream());
  }

protected:
  Tcp(std::shared_ptr<native::Loop> l);
  virtual void init(uv_handle_t *iHandlePtr) override;

private:
  static void Connect(uv_connect_t *req, int status);
  std::unique_ptr<uv_tcp_t> _uvTcpInstance;
  bool _connecting;
  bool _connected;
};

} /* namespace net */
} /* namespace native */

#endif /* __NATIVE_NET_TCP_HPP__ */
