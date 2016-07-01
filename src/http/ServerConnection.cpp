#include "native/http/ServerConnection.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerRequest.hpp"
#include "native/http/ServerResponse.hpp"

namespace native {
namespace http {

std::shared_ptr<ServerConnection> ServerConnection::Create(std::shared_ptr<Server> iServer) {
  std::shared_ptr<ServerConnection> instance(new ServerConnection(iServer));
  return instance;
}

ServerConnection::ServerConnection(std::shared_ptr<Server> iServer) : _server(iServer) {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_server);

  _socket = native::net::Tcp::Create(_server->getLoop());
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
    std::weak_ptr<ServerConnection> connectionWeak = getInstance();
    _response->onEnd([connectionWeak]() {
      std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
      for (std::function<void()> &cb : connection->_responseSentCbs) {
        cb();
      }
    });
  }

  return *_response;
}

Future<void> ServerConnection::close() {
  std::weak_ptr<ServerConnection> connectionWeak = getInstance();

  return this->_socket->close().then([connectionWeak](std::shared_ptr<base::Handle>) {
    std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
    for (std::function<void()> &cb : connection->_closeCbs) {
      cb();
    }
  });
}

void ServerConnection::parse() {
  NNATIVE_FCALL();
  // Create request and response if it is not yet created
  getRequest();

  std::weak_ptr<ServerConnection> connectionWeak = getInstance();

  _socket->readStart([connectionWeak](const char *buf, int len) {
    NNATIVE_ASSERT(!connectionWeak.expired());
    std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
    // NNATIVE_DEBUG("buff [" << buf << "], len: " << len);
    if ((buf == nullptr) || (len < 0)) {
      NNATIVE_DEBUG("received an negative length: " << len);

      if (len == UV_EOF) {
        // TODO: check if required to process the pending request
      } else if (len == UV_ECONNRESET) {
        NNATIVE_DEBUG("ECONNRESET");
        connection->close();
      } else {
        Error err = len;
        ServerResponse &response = connection->getResponse();
        if (!response.isSent()) {
          response.setStatus(500);
          std::string responseText("Invalid request format.");
          responseText += "Name: ";
          responseText += err.name();
          response.end(responseText);
          NNATIVE_DEBUG("Invalid request sent");
        }
      }

      return;
    }

    NNATIVE_DEBUG("start http_parser_execute");
    int parsed = connection->getRequest().parse(buf, len);
    if (!connection->getRequest().isUpgrade() && parsed != len) {
      // invalid request, close connection
      NNATIVE_DEBUG("HTTP parser error: " << connection->getRequest().getErrorName() << ". Close connection");
      connection->close();
    }

    if (connection->getRequest().isUpgrade() || connection->getRequest().isMessageComplete()) {
      NNATIVE_DEBUG("complete request");
      connection->processRequest();
    }

    NNATIVE_DEBUG("end http_parser_execute");
  });
}

void ServerConnection::processRequest() {
  for (std::function<void()> &cb : _requestReceivedCbs) {
    cb();
  }
  std::weak_ptr<ServerConnection> connectionWeak = getInstance();
  const std::string urlPath = getRequest().url().path();
  NNATIVE_DEBUG("execute path: " << urlPath);

  _server->execute(urlPath, getInstance())
      .then([connectionWeak]() {
        if (!connectionWeak.expired() && !connectionWeak.lock()->getResponse().isSent()) {
          NNATIVE_DEBUG("send 404");
          // send a 404 status
          http::ServerResponse &res = connectionWeak.lock()->getResponse();
          res.setStatus(404);
          res.end();
        }
      })
      .error([connectionWeak](const FutureError &err) {
        NNATIVE_DEBUG("ServerConnection error: " << err.message());
        if (connectionWeak.expired()) {
          NNATIVE_DEBUG("ServerConnection expired.");
          return;
        }

        std::shared_ptr<ServerConnection> connection = connectionWeak.lock();

        if (connection->getResponse().isSent()) {
          return;
        }

        NNATIVE_DEBUG("Trying to send 500 to client");

        // send a 500 status
        http::ServerResponse &res = connection->getResponse();
        res.setStatus(500);
        res.end(err.message());
      });
}

} /* namespace http */
} /* namespace native */
