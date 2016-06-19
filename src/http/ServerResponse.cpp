#include "native/http/ServerResponse.hpp"
#include "native/http/HttpUtils.hpp"
#include "native/http/ServerConnection.hpp"
#include "native/http/ServerRequest.hpp"

namespace native {
namespace http {

ServerResponse::ServerResponse(std::shared_ptr<ServerConnection> connection)
    : OutgoingMessage(connection->getRequest()), _statusCode(200), _connection(connection) {
  _headers["Content-Type"] = "text/html";
}

ServerResponse::~ServerResponse() {}

void ServerResponse::setHeaderFirstLine(std::stringstream &ioMessageRaw) const {
  ioMessageRaw << getHttpVersionString() << " " << _statusCode << " " << GetStatusText(_statusCode) << "\r\n";
}

void ServerResponse::setStatus(int status_code) { _statusCode = status_code; }

Future<void> ServerResponse::sendData(const std::string &str) { return _connection.lock()->_socket->write(str); }

Future<void> ServerResponse::writeData(const std::string &data) {

  if (!_headerSent) {
    if (_statusCode == 204 || _statusCode == 304) {
      if (_shouldKeepAlive) {
        NNATIVE_DEBUG(_statusCode << " should not use chunked enkoding");
      }

      _shouldKeepAlive = false;
      _shouldKeepAlive = false;
    }
  }

  return OutgoingMessage::writeData(data);
}

Future<void> ServerResponse::endData(const std::string &data) {
  std::weak_ptr<ServerConnection> transactionWeak = _connection;

  return OutgoingMessage::endData(data).then([transactionWeak]() {
    NNATIVE_DEBUG("close connection socket");
    auto transaction = transactionWeak.lock();
    // Promise<void> promise(transaction->getLoop());

    // if (transaction->_response->_shouldKeepAlive) {
    //  transaction->_request.reset();
    //  transaction->_response.reset();
    //  return
    //} else {
    return transaction->close();
    //}
  });
}

} /* namespace http */
} /* namespace native */
