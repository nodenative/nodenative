#include "native/Loop.hpp"
#include "native/http.hpp"

namespace native {
namespace http {

std::shared_ptr<Server> Server::Create() {
  std::shared_ptr<Loop> loop = Loop::GetInstanceSafe();
  std::shared_ptr<Server> instance(new Server(loop));
  instance->_instance = instance;
  return instance;
}

std::shared_ptr<Server> Server::Create(std::shared_ptr<Loop> iLoop) {
  std::shared_ptr<Server> instance(new Server(iLoop));
  instance->_instance = instance;
  return instance;
}

Server::Server(std::shared_ptr<Loop> iLoop)
    : ServerPlugin::ServerPlugin(iLoop), _loop(iLoop), _socket(native::net::Tcp::Create(iLoop)) {
  NNATIVE_FCALL();
}

Server::~Server() {
  NNATIVE_FCALL();
  if (_socket) {
    _socket->close();
  }
}

std::shared_ptr<Server> Server::getInstance() { return std::static_pointer_cast<Server>(ServerPlugin::getInstance()); }

void Server::onError(std::function<void(const native::Error &)> errorCb) { _errorCb = errorCb; }

void Server::error(const native::Error &e) {
  if (_errorCb) {
    _errorCb(e);
  } else {
    // unhandled error?
    throw e;
  }
}

bool Server::listen(const std::string &ip, int port) {
  NNATIVE_FCALL();
  if (!_socket->bind(ip, port)) {
    NNATIVE_DEBUG("failed to bind to ip " << ip << " port " << port);
    return false;
  }

  std::weak_ptr<Server> instanceWeak = getInstance();

  if (!_socket->listen([instanceWeak](native::Error e) {
        auto instance = instanceWeak.lock();
        NNATIVE_FCALL();
        NNATIVE_DEBUG("new connection");

        if (e) {
          NNATIVE_INFO("error: " << e.name() << ", str:" << e.str());
          instance->error(e);
          return;
        }
        std::shared_ptr<ServerConnection> connection = ServerConnection::Create(instanceWeak.lock());
        std::weak_ptr<ServerConnection> connectionWeak = connection;

        connection->parse()
            .then([connectionWeak, instanceWeak]() -> Future<void> {
              if (connectionWeak.expired()) {
                NNATIVE_DEBUG("ServerConnection expired.");
                return Promise<void>::Resolve(instanceWeak.lock()->_loop);
              }

              std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
              std::shared_ptr<Server> instance = instanceWeak.lock();

              const std::string urlPath = connection->_request->url().path();
              NNATIVE_INFO("execute path: " << urlPath);

              return instance->execute(urlPath, connection);
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
      })) {
    NNATIVE_DEBUG("failed to listen socket");
    return false;
  }

  return true;
}

Future<std::shared_ptr<Server>> Server::close() {
  std::weak_ptr<Server> instanceWeak = getInstance();

  return _socket->close().then([instanceWeak](std::shared_ptr<base::Handle>) -> std::shared_ptr<Server> {
    std::shared_ptr<Server> instance = instanceWeak.lock();
    instance->_instance.reset();
    return instance;
  });
}

} /* namespace http */
} /* namespace native */
