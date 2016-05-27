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
  _socket = net::Tcp::Create();
}

std::shared_ptr<ClientRequest>
ClientRequest::Create(std::shared_ptr<Loop> iLoop, const std::string &method, const std::string &uri) {
  UrlObject urlObj;
  urlObj.parse(uri);
  const std::string host = urlObj.host();
  const int port = urlObj.port();
  std::string path = urlObj.path();

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

Future<void> ClientRequest::endData(const std::string &data) {
  std::weak_ptr<ClientRequest> instanceWeak = _instance;

  return OutgoingMessage::endData(data).then([instanceWeak]() -> Future<void> {
    NNATIVE_DEBUG("close connection socket");
    return instanceWeak.lock()->_socket->close().then([](std::shared_ptr<base::Handle>) {});
  });
}

Future<std::shared_ptr<ClientResponse>> ClientRequest::end(const std::string &data) {
  // keep instance
  _instance = getInstance();
  _response.reset(new ClientResponse(_instance));
  std::weak_ptr<ClientRequest> instanceWeak = _instance;
  std::shared_ptr<std::string> resData(new std::string());

  return endData(data).then([instanceWeak]() -> Future<std::shared_ptr<ClientResponse>> {
    auto instance = instanceWeak.lock();
    Promise<std::shared_ptr<ClientResponse>> promise(instance->_loop);

    instance->_socket->readStart([promise, instanceWeak](const char *buf, ssize_t len) {
      auto instance = instanceWeak.lock();

      if ((buf == nullptr) || (len < 0)) {
        auto nonConstPromise = promise;
        nonConstPromise.reject("No response from socket");
        instance->_instance.reset();
        return;
      }

      const int parsed = instance->_response->parse(buf, len);

      if (!instance->_response->isUpgrade() && parsed != len) {
        // invalid request, close connection
        NNATIVE_INFO("HTTP parser error: " << instance->_response->getErrorName() << ". Close transaction");
        auto nonConstPromise = promise;
        nonConstPromise.reject(instance->_response->getErrorName());
        instance->_instance.reset();
        instance->_socket->close().then(
            [promise, instanceWeak](std::shared_ptr<base::Handle>) { instanceWeak.lock()->_connected = false; });
        return;
      }

      if (instance->_response->isUpgrade() || instance->_response->isMessageComplete()) {
        NNATIVE_DEBUG("complete request");
        instance->_instance.reset();
        auto nonConstPromise = promise;
        nonConstPromise.resolve(instance->_response);
      }
    });

    return promise.getFuture();
  });
}

} /* namespace http */
} /* namespace native */
