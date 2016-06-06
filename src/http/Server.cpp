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

void Server::onCreateConnection(
    std::function<std::shared_ptr<ServerConnection>(std::shared_ptr<Server>)> createConnectionCb) {
  _createConnectionCb = createConnectionCb;
}

std::shared_ptr<ServerConnection> Server::createConnection() {
  std::shared_ptr<ServerConnection> connection;
  if (_createConnectionCb) {
    connection = _createConnectionCb(getInstance());
  }

  // If callback refuze to create connection, then create a default connection
  if (!connection) {
    connection = ServerConnection::Create(getInstance());
  }

  return connection;
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

        instance->createConnection()->parse();
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
