#ifndef __NATIVE_HTTP_SERVER_HPP__
#define __NATIVE_HTTP_SERVER_HPP__

#include "../net.hpp"
#include "ServerPlugin.hpp"
#include "Transaction.hpp"

namespace native {

namespace http {
/**
 * HTTP Server class. See example:
 * @example webserver.cpp
 */
class Server : public ServerPlugin {
  friend class Transaction;
  friend class ServerPlugin;

protected:
  Server(std::shared_ptr<Loop> iLoop);
  std::shared_ptr<ServerPlugin> getDefaultPlugin();

public:
  Server() = delete;
  virtual ~Server();

  bool listen(const std::string &ip, int port, std::function<void(TransactionInstance)> callback);
  Future<std::shared_ptr<Server>> close();

  static std::shared_ptr<Server> Create();
  static std::shared_ptr<Server> Create(std::shared_ptr<Loop> iLoop);
  std::shared_ptr<Server> getInstance();

protected:
  std::shared_ptr<Loop> _loop;
  std::shared_ptr<native::net::Tcp> _socket;
  std::shared_ptr<Server> _instance;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVER_HPP__ */
