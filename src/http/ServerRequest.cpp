#include "native/http/ServerRequest.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerConnection.hpp"
#include "native/http/ServerResponse.hpp"

namespace native {
namespace http {

ServerRequest::ServerRequest(std::shared_ptr<ServerConnection> iTransaction)
    : IncomingMessage(true), _connection(iTransaction), _complete(false) {}

ServerRequest::~ServerRequest() {
  // printf("~ServerRequest() %x\n", this);
}

void ServerRequest::onMessageComplete() { _complete = true; }

} /* namespace http */
} /* namespace native */
