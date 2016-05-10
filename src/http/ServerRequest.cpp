#include "native/http/ServerRequest.hpp"
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

  _callback(_transaction.lock());
}

} /* namespace http */
} /* namespace native */
