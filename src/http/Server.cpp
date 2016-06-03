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

bool Server::listen(const std::string &ip, int port) {
  NNATIVE_FCALL();
  return listen(ip, port, std::function<void(std::shared_ptr<Transaction>)>());
}

bool Server::listen(const std::string &ip, int port, std::function<void(std::shared_ptr<Transaction>)> callback) {
  NNATIVE_FCALL();
  if (!_socket->bind(ip, port)) {
    NNATIVE_DEBUG("failed to bind to ip " << ip << " port " << port);
    return false;
  }

  _callback = callback;

  std::weak_ptr<Server> instanceWeak = getInstance();

  if (!_socket->listen([instanceWeak](native::Error e) {
        NNATIVE_FCALL();
        NNATIVE_DEBUG("start listen");
        if (e) {
          // TODO: handle client connection Error
          NNATIVE_INFO("error: " << e.name() << ", str:" << e.str());
        } else {
          std::shared_ptr<Transaction> transaction = Transaction::Create(instanceWeak.lock());
          std::weak_ptr<Transaction> transactionWeak = transaction;

          transaction->parse()
              .then([transactionWeak, instanceWeak]() -> Future<void> {
                if (transactionWeak.expired()) {
                  NNATIVE_DEBUG("Transaction expired.");
                  return Promise<void>::Resolve(instanceWeak.lock()->_loop);
                }

                std::shared_ptr<Transaction> transaction = transactionWeak.lock();
                std::shared_ptr<Server> instance = instanceWeak.lock();

                if (instance->_callback) {
                  instance->_callback(transaction);
                }

                if (transaction->getResponse().isSent()) {
                  NNATIVE_DEBUG("response sent. skip ServerPlugin");
                  return Promise<void>::Resolve(instanceWeak.lock()->_loop);
                }

                const std::string urlPath = transaction->_request->url().path();
                NNATIVE_DEBUG("execute path: " << urlPath);

                return instance->execute(urlPath, transaction);
              })
              .then([transactionWeak]() {
                if (!transactionWeak.expired() && !transactionWeak.lock()->getResponse().isSent()) {
                  // send a 404 status
                  http::ServerResponse &res = transactionWeak.lock()->getResponse();
                  res.setStatus(404);
                  res.end();
                }
              })
              .error([transactionWeak](const FutureError &err) {
                NNATIVE_INFO("Transaction error: " << err.message());
                if (transactionWeak.expired()) {
                  NNATIVE_INFO("Transaction expired.");
                  return;
                }

                std::shared_ptr<Transaction> transaction = transactionWeak.lock();

                if (transaction->getResponse().isSent()) {
                  return;
                }

                NNATIVE_INFO("Trying to send 500 to client");

                // send a 500 status
                http::ServerResponse &res = transaction->getResponse();
                res.setStatus(500);
                res.end(err.message());
              });
        }
        NNATIVE_DEBUG("end listen");
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
