#include "native/http/Transaction.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerRequest.hpp"
#include "native/http/ServerResponse.hpp"

namespace native {
namespace http {

std::shared_ptr<Transaction> Transaction::Create(std::shared_ptr<Server> iServer) {
  std::shared_ptr<Transaction> instance(new Transaction(iServer));
  instance->_instance = instance;
  return instance;
}

Transaction::Transaction(std::shared_ptr<Server> iServer) : _server(iServer) {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_server);

  _socket = native::net::Tcp::Create();
  NNATIVE_ASSERT(_server->_socket->accept(_socket));
}

Transaction::~Transaction() {
  NNATIVE_FCALL();
  if (_socket.use_count() == 1 && _socket->isActive()) {
    NNATIVE_DEBUG("close transaction socket from destructor");
    _socket->close();
  }
}

std::unique_ptr<ServerRequest> Transaction::createRequest() {
  return std::unique_ptr<ServerRequest>(new ServerRequest(getInstance()));
}

std::unique_ptr<ServerResponse> Transaction::createResponse() {
  return std::unique_ptr<ServerResponse>(new ServerResponse(getInstance()));
}

ServerRequest &Transaction::getRequest() {
  if (!_request) {
    _request = createRequest();
  }

  return *_request;
}

ServerResponse &Transaction::getResponse() {
  if (!_response) {
    _response = createResponse();
  }

  return *_response;
}

Future<void> Transaction::close() {
  std::weak_ptr<Transaction> transactionWeak = this->getInstance();
  return this->_socket->close().then([transactionWeak](std::shared_ptr<base::Handle>) {
    std::shared_ptr<Transaction> instance = transactionWeak.lock();
    instance->_instance.reset();
  });
}

bool Transaction::parse(std::function<void(std::shared_ptr<Transaction>)> callback) {
  NNATIVE_FCALL();
  // Create request and response if it is not yet created
  getRequest();
  getResponse();
  _request->initParser(callback);

  std::weak_ptr<Transaction> instanceWeak = getInstance();

  _socket->readStart([instanceWeak](const char *buf, int len) {
    NNATIVE_ASSERT(!instanceWeak.expired());
    std::shared_ptr<Transaction> instance = instanceWeak.lock();
    // NNATIVE_DEBUG("buff [" << buf << "], len: " << len);
    // TODO: resolve multi part
    if ((buf == nullptr) || (len <= 0)) {
      instance->_response->setStatus(500);
      instance->_response->end("Invalid request format.");
    } else {
      NNATIVE_DEBUG("start http_parser_execute");
      instance->_request->parse(buf, len);
      NNATIVE_DEBUG("end http_parser_execute");
    }
    // instance.reset();
  });

  return true;
}

} /* namespace http */
} /* namespace native */
