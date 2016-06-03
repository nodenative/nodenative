#include "native/http/OutgoingMessage.hpp"
#include "native/http/HttpUtils.hpp"

namespace native {
namespace http {

OutgoingMessage::OutgoingMessage()
    : _headerSent(false), _closed(false), _sent(false), _last(false), _chunkedEncoding(false),
      _useChunkedEncodingByDefault(false), _sendDate(false), _shouldKeepAlive(true), _contentLength(-1),
      _hasBody(false) {
  _headers["Content-Type"] = "text/html";
}

OutgoingMessage::~OutgoingMessage() {}

std::string OutgoingMessage::getMessageHeaderRaw() const {
  std::stringstream message_raw;
  // first line in the case of request is: 'GET /index.html HTTP/1.1\r\n'
  // in the case of response it is: 'HTTP/1.1 200 OK\r\n'
  setHeaderFirstLine(message_raw);

  for (auto h : _headers) {
    message_raw << h.first << ": " << h.second << "\r\n";
  }
  message_raw << "\r\n";

  return message_raw.str();
}

Future<void> OutgoingMessage::writeData(const std::string &str) {
  NNATIVE_ASSERT(!_closed);
  Future<void> future(Loop::GetInstanceSafe());

  if (!_headerSent) {
    if (!str.empty() && _headers.find("Content-Length") == _headers.end()) {
      std::stringstream ss;
      ss << str.length();
      _headers["Content-Length"] = ss.str();
      // mark as closed because content length is reached
      _closed = true;
      _sent = true;
    }

    _headerSent = true;
    std::string str = getMessageHeaderRaw();
    future = sendData(str);
  } else {
    future = Promise<void>::Resolve(Loop::GetInstanceSafe());
  }

  if (!str.empty()) {
    OutgoingMessage *messagePtr = this;

    future = future.then([messagePtr, str]() { return messagePtr->sendData(str); });
  }

  return future;
}

Future<void> OutgoingMessage::endData(const std::string &str) {
  NNATIVE_ASSERT(!_closed);
  Future<void> future = writeData(str);
  _closed = true;
  _sent = true;
  return future;
}

void OutgoingMessage::setHeader(const std::string &key, const std::string &value) { _headers[key] = value; }

} /* namespace http */
} /* namespace native */
