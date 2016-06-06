#include "native/http/ServerConnection.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerRequest.hpp"
#include "native/http/ServerResponse.hpp"

namespace native {
namespace http {

std::shared_ptr<ServerConnection> ServerConnection::Create(std::shared_ptr<Server> iServer) {
  std::shared_ptr<ServerConnection> instance(new ServerConnection(iServer));
  instance->_instance = instance;
  return instance;
}

ServerConnection::ServerConnection(std::shared_ptr<Server> iServer) : _server(iServer) {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_server);

  _socket = native::net::Tcp::Create();
  NNATIVE_ASSERT(_server->_socket->accept(_socket));
}

ServerConnection::~ServerConnection() {
  NNATIVE_FCALL();
  if (_socket.use_count() == 1 && _socket->isActive()) {
    NNATIVE_DEBUG("close connection socket from destructor");
    _socket->close();
  }
}

std::unique_ptr<ServerRequest> ServerConnection::createRequest() {
  return std::unique_ptr<ServerRequest>(new ServerRequest(getInstance()));
}

std::unique_ptr<ServerResponse> ServerConnection::createResponse() {
  return std::unique_ptr<ServerResponse>(new ServerResponse(getInstance()));
}

ServerRequest &ServerConnection::getRequest() {
  if (!_request) {
    _request = createRequest();
  }

  return *_request;
}

ServerResponse &ServerConnection::getResponse() {
  if (!_response) {
    _response = createResponse();
  }

  return *_response;
}

Future<void> ServerConnection::close() {
  std::weak_ptr<ServerConnection> transactionWeak = this->getInstance();
  return this->_socket->close().then([transactionWeak](std::shared_ptr<base::Handle>) {
    std::shared_ptr<ServerConnection> instance = transactionWeak.lock();
    instance->_instance.reset();
  });
}

void ServerConnection::parse() {
  NNATIVE_FCALL();
  // Create request and response if it is not yet created
  getRequest();
  getResponse();

  Promise<void> promise(_socket->getLoop());
  std::weak_ptr<ServerConnection> connectionWeak = getInstance();

  _socket->readStart([connectionWeak, promise](const char *buf, int len) {
    NNATIVE_ASSERT(!connectionWeak.expired());
    std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
    // NNATIVE_DEBUG("buff [" << buf << "], len: " << len);
    if ((buf == nullptr) || (len < 0)) {
      NNATIVE_INFO("received an negative length: " << len);
      if (!connection->_response->isSent()) {
        connection->_response->setStatus(500);
        connection->_response->end("Invalid request format.");
        NNATIVE_INFO("Invalid request sent");
      }
      return;
    }

    NNATIVE_DEBUG("start http_parser_execute");
    int parsed = connection->_request->parse(buf, len);
    if (!connection->_request->isUpgrade() && parsed != len) {
      // invalid request, close connection
      NNATIVE_INFO("HTTP parser error: " << connection->_request->getErrorName() << ". Close connection");
      connection->close()
          .then([promise]() {
            Promise<void> nonConstPromise = promise;
            nonConstPromise.resolve();
          })
          .error([promise](const FutureError &err) {
            Promise<void> nonConstPromise = promise;
            nonConstPromise.resolve();
          });
    }

    if (connection->_request->isUpgrade() || connection->_request->isMessageComplete()) {
      NNATIVE_DEBUG("complete request");
      Promise<void> nonConstPromise = promise;
      nonConstPromise.resolve();
    }

    NNATIVE_DEBUG("end http_parser_execute");
  });

  promise.getFuture()
      .then([connectionWeak]() -> Future<void> {
        NNATIVE_ASSERT(!connectionWeak.expired());
        std::shared_ptr<ServerConnection> connection = connectionWeak.lock();

        const std::string urlPath = connection->_request->url().path();
        NNATIVE_INFO("execute path: " << urlPath);

        return connection->_server->execute(urlPath, connection);
      })
      .then([connectionWeak]() {
        if (!connectionWeak.expired() && !connectionWeak.lock()->getResponse().isSent()) {
          // send a 404 status
          http::ServerResponse &res = connectionWeak.lock()->getResponse();
          res.setStatus(404);
          res.end();
        }
      })
      .error([connectionWeak](const FutureError &err) {
        NNATIVE_INFO("ServerConnection error: " << err.message());
        if (connectionWeak.expired()) {
          NNATIVE_INFO("ServerConnection expired.");
          return;
        }

        std::shared_ptr<ServerConnection> connection = connectionWeak.lock();

        if (connection->getResponse().isSent()) {
          return;
        }

        NNATIVE_INFO("Trying to send 500 to client");

        // send a 500 status
        http::ServerResponse &res = connection->getResponse();
        res.setStatus(500);
        res.end(err.message());
      });
}

} /* namespace http */
} /* namespace native */
