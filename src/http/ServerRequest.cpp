#include "native/http/ServerRequest.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerResponse.hpp"
#include "native/http/Transaction.hpp"

namespace native {
namespace http {

ServerRequest::ServerRequest(std::shared_ptr<Transaction> iTransaction)
    : IncomingMessage(true), _transaction(iTransaction), _complete(false) {}

ServerRequest::~ServerRequest() {
  // printf("~ServerRequest() %x\n", this);
}

void ServerRequest::onMessageComplete() { _complete = true; }

} /* namespace http */
} /* namespace native */
