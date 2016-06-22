#include "native/http/OutgoingMessage.hpp"
#include "native/http/HttpUtils.hpp"
#include "native/http/IncomingMessage.hpp"

namespace native {
namespace http {

OutgoingMessage::OutgoingMessage()
    : _headerSent(false), _closed(false), _sent(false), _last(false), _chunkedEncoding(true),
      _useChunkedEncodingByDefault(false), _sendDate(false), _shouldKeepAlive(true), _contentLength(-1),
      _hasBody(false) {
  _headers["Content-Type"] = "text/html";
}

OutgoingMessage::OutgoingMessage(const IncomingMessage &incomming)
    : MessageBase(incomming), _headerSent(false), _closed(false), _sent(false), _last(false), _chunkedEncoding(false),
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
  std::string data;

  if (!_headerSent) {
    // If the body is sent in 1 go, then deactivate chunked enkoding.
    if (_last) {
      _chunkedEncoding = false;
    }

    if (!_chunkedEncoding && _headers.find("Content-Length") == _headers.end()) {
      std::stringstream ss;
      ss << str.length();
      _headers["Content-Length"] = ss.str();
      // mark as closed because content length is reached
      _closed = true;
      _sent = true;
    }

    if (_shouldKeepAlive) {
      _headers["Connection"] = "keep-alive";
    } else {
      _headers["Connection"] = "close";
    }

    if (_chunkedEncoding) {
      _headers["Transfer-Encoding"] = "chunked";
    } else {
      // remove chunk encoding if exists
    }

    _headerSent = true;
    data = getMessageHeaderRaw();
    NNATIVE_DEBUG("send header " << data.size());
  }

  if (!str.empty()) {
    data += str;
  }

  return sendData(data);
}

Future<void> OutgoingMessage::endData(const std::string &str) {
  NNATIVE_ASSERT(!_closed);
  NNATIVE_ASSERT(!_last);

  _last = true;

  Future<void> future = writeData(str);
  _closed = true;
  _sent = true;
  return future;
}

void OutgoingMessage::setHeader(const std::string &key, const std::string &value) { _headers[key] = value; }

} /* namespace http */
} /* namespace native */
