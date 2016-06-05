#ifndef __NATIVE_HTTP_SERVER_HPP__
#define __NATIVE_HTTP_SERVER_HPP__

#include "../net.hpp"
#include "ServerConnection.hpp"
#include "ServerPlugin.hpp"

namespace native {

namespace http {
/**
 * HTTP Server class. See example:
 * @example webserver.cpp
 */
class Server : public ServerPlugin {
  friend class ServerConnection;
  friend class ServerPlugin;

protected:
  Server(std::shared_ptr<Loop> iLoop);
  std::shared_ptr<ServerPlugin> getDefaultPlugin();

public:
  Server() = delete;
  virtual ~Server();

  bool listen(const std::string &ip, int port);
  Future<std::shared_ptr<Server>> close();

  static std::shared_ptr<Server> Create();
  static std::shared_ptr<Server> Create(std::shared_ptr<Loop> iLoop);
  std::shared_ptr<Server> getInstance();

  /**
   * Add error callback, `errorCb`.
   *
   * @param errorCb error callback
   */
  void onError(std::function<void(const native::Error &)> errorCb);

  /**
   * Send an error to Server
   * @param error error data
   */
  void error(const native::Error &error);

protected:
  std::shared_ptr<Loop> _loop;
  std::shared_ptr<native::net::Tcp> _socket;
  std::shared_ptr<Server> _instance;
  std::function<void(const native::Error &)> _errorCb;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVER_HPP__ */
