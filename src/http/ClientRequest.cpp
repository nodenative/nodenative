#include "native/http/ClientRequest.hpp"
#include "native/http/ClientResponse.hpp"
#include "native/http/UrlObject.hpp"

namespace native {
namespace http {

ClientRequest::ClientRequest(std::shared_ptr<Loop> iLoop,
                             const std::string &method,
                             const std::string &host,
                             const int &port,
                             const std::string &path)
    : _method(method), _host(host), _port(port), _path(path), _loop(iLoop), _connected(false) {
  _shouldKeepAlive = true;
  if (_path.empty()) {
    _path = "/";
  }
}

ClientRequest::~ClientRequest() {
  if (_socket) {
    NNATIVE_DEBUG("Close socket");
    _socket->close();
  }
}

std::shared_ptr<ClientRequest>
ClientRequest::Create(std::shared_ptr<Loop> iLoop, const std::string &method, const std::string &uri) {
  UrlObject urlObj;
  urlObj.parse(uri);
  std::string host = urlObj.host();
  const int port = urlObj.port();
  std::string path = urlObj.path();
  const std::string query = urlObj.query();

  if (!query.empty()) {
    path += "?" + query;
  }

  return Create(iLoop, method, host, port, path);
}

std::shared_ptr<ClientRequest> ClientRequest::Create(std::shared_ptr<Loop> iLoop,
                                                     const std::string &method,
                                                     const std::string &host,
                                                     const int &port,
                                                     const std::string &path) {
  std::shared_ptr<ClientRequest> instance(new ClientRequest(iLoop, method, host, port, path));
  return instance;
}

void ClientRequest::setHeaderFirstLine(std::stringstream &ioMessageRaw) const {
  ioMessageRaw << _method << " " << _path << " " << getHttpVersionString() << "\r\n";
}

Future<void> ClientRequest::sendData(const std::string &str) {
  if (_connected) {
    return _socket->write(str);
  }

  std::weak_ptr<ClientRequest> instanceWeak = _instance;

  return _socket->connect(_host, _port).then([str, instanceWeak]() { instanceWeak.lock()->_socket->write(str); });
}

Future<void> ClientRequest::endData(const std::string &data) { return OutgoingMessage::endData(data); }

Future<std::shared_ptr<ClientResponse>> ClientRequest::end(const std::string &data) {
  // keep instance
  _instance = getInstance();
  _response.reset(new ClientResponse(_instance));
  NNATIVE_ASSERT(!_socket);
  _socket = net::Tcp::Create(_loop);
  std::weak_ptr<ClientRequest> instanceWeak = _instance;

  return endData(data).then([instanceWeak]() -> Future<std::shared_ptr<ClientResponse>> {
    auto instance = instanceWeak.lock();
    Promise<std::shared_ptr<ClientResponse>> promise(instance->_loop);

    instance->_socket->readStart([promise, instanceWeak](const char *buf, ssize_t len) {
      NNATIVE_DEBUG("Received len: " << len << ", expired: " << instanceWeak.expired());
      auto instance = instanceWeak.lock();
      auto nonConstPromise = promise;
      NNATIVE_ASSERT(instance->_instance);

      if ((buf == nullptr) || (len < 0)) {
        auto nonConstPromise = promise;
        if (len == UV_EOF && !instance->_response->isMessageComplete()) {
          NNATIVE_DEBUG("resolve client response because of EOF");
          nonConstPromise.resolve(instance->_response);
        } else {
          Error err = len;
          NNATIVE_DEBUG("No response from socket: " << len << ", name: " << err.name() << ", str:" << err.str());
          nonConstPromise.reject("No response from socket");
        }
        instance->_instance.reset();

        instance->_socket->readStop();
        // TODO: keep the connection and pass to future requests. Keep-alive
        instance->_socket->close();
        instance->_socket.reset();
        return;
      }

      const int parsed = instance->_response->parse(buf, len);

      if (!instance->_response->isUpgrade() && parsed != len) {
        // invalid request, close connection
        NNATIVE_DEBUG("HTTP parser error: " << instance->_response->getErrorName() << ". Close transaction");
        nonConstPromise.reject(instance->_response->getErrorName());
        instance->_instance.reset();
        instance->_socket->readStop();
        instanceWeak.lock()->_connected = false;
        instance->_socket->close();
        instance->_socket.reset();
        return;
      }

      if (instance->_response->isUpgrade() || instance->_response->isMessageComplete()) {
        NNATIVE_DEBUG("complete request");
        nonConstPromise.resolve(instance->_response);
        instance->_instance.reset();
        instance->_response.reset();
        instance->_socket->readStop();
        instance->_socket->close();
        instance->_socket.reset();
      }
    });

    return promise.getFuture();
  });
}

} /* namespace http */
} /* namespace native */
