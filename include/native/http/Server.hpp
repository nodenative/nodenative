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
  std::shared_ptr<Loop> getLoop() { return _loop; }

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

  /**
   * Add create connection callback.
   *
   * if callback refuze to create connection, then a default connection will be created.
   *
   * @param createConnectionCb create connection callback
   */
  void onCreateConnection(std::function<std::shared_ptr<ServerConnection>(std::shared_ptr<Server>)> createConnectionCb);

  /**
   * Create connection method. This method usually is called when a new connection is created.
   *
   * @return new connection
   */
  std::shared_ptr<ServerConnection> createConnection();

  /**
   * Set the idle timeout.
   * Idle timeout is the time between the connection is created and the request is received for the first request,
   * or the time between a sent response and the next received request.
   * The new timeout will take efect only for new connections.
   *
   * If the timeout is reached, the connection is closed.
   *
   * @param seconds timeout in seconds. Default value is 30 seconds
   */
  void setIdleConnectionTimeout(const uint64_t &seconds = 30) {
    NNATIVE_ASSERT(seconds > 0);
    _idleConnectionTimeoutSeconds = seconds;
  }

  const uint64_t &getIdleConnectionTimeoutSeconds() const { return _idleConnectionTimeoutSeconds; }

  /**
   * Set response timeout in seconds.
   * If the timeout is reached, the connection is closed.
   * @param seconds timeout value in seconds. Default value is 60 seconds
   */
  void setResponseTimeout(const uint64_t &seconds = 60) {
    NNATIVE_ASSERT(seconds > 0);
    _responseTimeoutSeconds = seconds;
  }
  const uint64_t &getResponseTimeoutSeconds() const { return _responseTimeoutSeconds; }

protected:
  std::shared_ptr<Loop> _loop;
  std::shared_ptr<native::net::Tcp> _socket;
  std::shared_ptr<Server> _instance;
  std::function<void(const native::Error &)> _errorCb;
  std::list<std::shared_ptr<ServerConnection>> _connections;
  std::function<std::shared_ptr<ServerConnection>(std::shared_ptr<Server>)> _createConnectionCb;
  uint64_t _idleConnectionTimeoutSeconds;
  uint64_t _responseTimeoutSeconds;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVER_HPP__ */
