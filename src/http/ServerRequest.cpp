#include "native/http/ServerRequest.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerResponse.hpp"
#include "native/http/Transaction.hpp"

namespace native {
namespace http {

ServerRequest::ServerRequest(std::shared_ptr<Transaction> iTransaction)
    : IncomingMessage(true), _transaction(iTransaction) {}

ServerRequest::~ServerRequest() {
  // printf("~ServerRequest() %x\n", this);
}

void ServerRequest::initParser(std::function<void(std::shared_ptr<Transaction>)> callback) {
  NNATIVE_FCALL();

  // store callback object
  _callback = callback;

  IncomingMessage::initParser();
}

void ServerRequest::onMessageComplete() {
  NNATIVE_FCALL();

  if (_callback) {
    _callback(_transaction.lock());
  }

  if (_transaction.lock()->getResponse().isSent()) {
    return;
  }

  _transaction.lock()->_server->execute(url().path(), _transaction.lock());
}

} /* namespace http */
} /* namespace native */
