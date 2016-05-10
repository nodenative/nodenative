#include "native/http/IncomingMessage.hpp"

namespace native {
namespace http {

IncomingMessage::IncomingMessage(const bool iIsRequest)
    : _isRequest(iIsRequest), _httpMajor(0), _httpMinor(0), _method(0), _statusCode(16), _wasHeaderValue(true) {}

IncomingMessage::~IncomingMessage() {}

const std::string &IncomingMessage::getHeader(const std::string &key) const {
  auto it = _headers.find(key);
  if (it != _headers.end())
    return it->second;
  return _defaultValue;
}

bool IncomingMessage::getHeader(const std::string &key, std::string &value) const {
  auto it = _headers.find(key);
  if (it != _headers.end()) {
    value = it->second;
    return true;
  }
  return false;
}

void IncomingMessage::initParser() {
  NNATIVE_FCALL();

  http_parser_init(&_parser, HTTP_REQUEST);
  _parser.data = this;

  _parserSettings.on_message_begin = [](http_parser *parser) {
    NNATIVE_DEBUG("message begin")
    return 0;
  };

  _parserSettings.on_url = [](http_parser *parser, const char *at, size_t len) {
    auto client = reinterpret_cast<IncomingMessage *>(parser->data);

    //  TODO: fromBuf() can throw an exception: check
    client->_url.fromBuf(at, len);

    return 0;
  };
  _parserSettings.on_header_field = [](http_parser *parser, const char *at, size_t len) {
    auto client = reinterpret_cast<IncomingMessage *>(parser->data);

    if (client->_wasHeaderValue) {
      // new field started
      if (!client->_lastHeaderField.empty()) {
        // add new entry
        client->_headers[client->_lastHeaderField] = client->_lastHeaderValue;
        client->_lastHeaderValue.clear();
      }

      client->_lastHeaderField = std::string(at, len);
      client->_wasHeaderValue = false;
    } else {
      // appending
      client->_lastHeaderField += std::string(at, len);
    }
    return 0;
  };
  _parserSettings.on_header_value = [](http_parser *parser, const char *at, size_t len) {
    auto client = reinterpret_cast<IncomingMessage *>(parser->data);

    if (!client->_wasHeaderValue) {
      client->_lastHeaderValue = std::string(at, len);
      client->_wasHeaderValue = true;
    } else {
      // appending
      client->_lastHeaderValue += std::string(at, len);
    }
    return 0;
  };
  _parserSettings.on_headers_complete = [](http_parser *parser) {
    auto client = reinterpret_cast<IncomingMessage *>(parser->data);

    // add last entry if any
    if (!client->_lastHeaderField.empty()) {
      // add new entry
      client->_headers[client->_lastHeaderField] = client->_lastHeaderValue;
    }

    client->_httpMajor = parser->http_major;
    client->_httpMinor = parser->http_minor;

    if (client->_isRequest) {
      client->_method = parser->method;
    } else {
      client->_statusCode = parser->status_code;
      // if HEAD, 1xx, 204, 304, return 1 which means F_SKIPBODY
      if ((client->_statusCode / 100) == 1 || client->_statusCode == 204 || client->_statusCode == 304) {
        return 1;
      }
    }

    return 0; // 1 to prevent reading of message body.
  };
  _parserSettings.on_body = [](http_parser *parser, const char *at, size_t len) {
    // printf("on_body, len of 'char* at' is %d\n", len);
    auto client = reinterpret_cast<IncomingMessage *>(parser->data);
    client->_body = std::string(at, len);
    return 0;
  };
  _parserSettings.on_message_complete = [](http_parser *parser) {
    NNATIVE_DEBUG("on_message_complete, so invoke the callback.\n");
    auto client = reinterpret_cast<IncomingMessage *>(parser->data);
    client->onMessageComplete();
    return 0; // 0 or 1?
  };
}

void IncomingMessage::parse(const char *buf, int len) { http_parser_execute(&_parser, &_parserSettings, buf, len); }

} /* namespace http */
} /* namespace native */
