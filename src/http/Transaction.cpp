#include "native/http/Transaction.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerRequest.hpp"
#include "native/http/ServerResponse.hpp"

namespace native {
namespace http {

TransactionInstance Transaction::Create(std::shared_ptr<Server> iServer) {
  TransactionInstance instance(new Transaction(iServer));
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

Future<void> Transaction::parse() {
  NNATIVE_FCALL();
  // Create request and response if it is not yet created
  getRequest();
  getResponse();

  Promise<void> promise(_socket->getLoop());
  std::weak_ptr<Transaction> instanceWeak = getInstance();

  _socket->readStart([instanceWeak, promise](const char *buf, int len) {
    NNATIVE_ASSERT(!instanceWeak.expired());
    std::shared_ptr<Transaction> instance = instanceWeak.lock();
    // NNATIVE_DEBUG("buff [" << buf << "], len: " << len);
    if ((buf == nullptr) || (len < 0)) {
      NNATIVE_INFO("received an negative length: " << len);
      if (!instance->_response->isSent()) {
        instance->_response->setStatus(500);
        instance->_response->end("Invalid request format.");
      }
      return;
    }

    NNATIVE_DEBUG("start http_parser_execute");
    int parsed = instance->_request->parse(buf, len);
    if (!instance->_request->isUpgrade() && parsed != len) {
      // invalid request, close connection
      NNATIVE_INFO("HTTP parser error: " << instance->_request->getErrorName() << ". Close transaction");
      instance->close()
          .then([promise]() {
            Promise<void> nonConstPromise = promise;
            nonConstPromise.resolve();
          })
          .error([promise](const FutureError &err) {
            Promise<void> nonConstPromise = promise;
            nonConstPromise.resolve();
          });
    }

    if (instance->_request->isUpgrade() || instance->_request->isMessageComplete()) {
      NNATIVE_DEBUG("complete request");
      Promise<void> nonConstPromise = promise;
      nonConstPromise.resolve();
    }

    NNATIVE_DEBUG("end http_parser_execute");
    // instance.reset();
  });

  return promise.getFuture();
}

} /* namespace http */
} /* namespace native */
