#include "native/http/ServerResponse.hpp"
#include "native/http/HttpUtils.hpp"
#include "native/http/Transaction.hpp"

namespace native {
namespace http {

ServerResponse::ServerResponse(std::shared_ptr<Transaction> iTransaction)
    : OutgoingMessage(false), _transaction(iTransaction) {
  _headers["Content-Type"] = "text/html";
}

ServerResponse::~ServerResponse() {}

Future<void> ServerResponse::send(const std::string &str) { return _transaction.lock()->_socket->write(str); }

Future<void> ServerResponse::end(const std::string &data) {
  std::weak_ptr<Transaction> transactionWeak = _transaction;

  return OutgoingMessage::end(data).then([transactionWeak]() {
    NNATIVE_DEBUG("close transaction socket");
    return transactionWeak.lock()->close();
  });
}

} /* namespace http */
} /* namespace native */
