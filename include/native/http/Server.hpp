#ifndef __NATIVE_HTTP_SERVER_HPP__
#define __NATIVE_HTTP_SERVER_HPP__

#include "../net.hpp"

namespace native {

namespace http {

class Transaction;

/**
 * HTTP Server class. See example:
 * @example webserver.cpp
 */
class Server : public std::enable_shared_from_this<Server> {
  friend class Transaction;

protected:
  Server(std::shared_ptr<Loop> iLoop);

public:
  Server() = delete;
  virtual ~Server();

  bool listen(const std::string &ip, int port, std::function<void(std::shared_ptr<Transaction>)> callback);
  Future<std::shared_ptr<Server>> close();

  static std::shared_ptr<Server> Create();
  static std::shared_ptr<Server> Create(std::shared_ptr<Loop> iLoop);
  std::shared_ptr<Server> getInstance();

protected:
  std::shared_ptr<native::net::Tcp> _socket;
  std::shared_ptr<Server> _instance;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVER_HPP__ */
