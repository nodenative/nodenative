#ifndef __NATIVE_HTTP_TRANSACTION_HPP__
#define __NATIVE_HTTP_TRANSACTION_HPP__

#include "../net/Tcp.hpp"

#include <http_parser.h>
#include <memory>

namespace native {

namespace http {

class Server;
class ServerRequest;
class ServerResponse;

class Transaction;

typedef std::shared_ptr<Transaction> TransactionInstance;

class Transaction : public std::enable_shared_from_this<Transaction> {
  friend class ServerRequest;
  friend class ServerResponse;
  friend class Server;
  friend class ServerPlugin;

protected:
  Transaction(std::shared_ptr<Server> server);

public:
  static TransactionInstance Create(std::shared_ptr<Server> iServer);
  ~Transaction();

  std::shared_ptr<Transaction> getInstance() { return this->shared_from_this(); }

  ServerRequest &getRequest();
  ServerResponse &getResponse();

  Future<void> close();

private:
  Future<void> parse();

protected:
  virtual std::unique_ptr<ServerRequest> createRequest();
  virtual std::unique_ptr<ServerResponse> createResponse();

  TransactionInstance _instance;
  std::shared_ptr<Server> _server;
  std::shared_ptr<native::net::Tcp> _socket;
  std::unique_ptr<ServerRequest> _request;
  std::unique_ptr<ServerResponse> _response;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_TRANSACTION_HPP__ */
