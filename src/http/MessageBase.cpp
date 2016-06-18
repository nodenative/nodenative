#include "native/http/MessageBase.hpp"

namespace native {
namespace http {

// Set default HTTP/1.1
// TODO: Consider to migrate to HTTP/2 by default. It requires to implement the connection negotiation first over
// HTTP/1.1
MessageBase::MessageBase() : _httpMajor(1), _httpMinor(1) {}

MessageBase::MessageBase(const MessageBase &other) : _httpMajor(other._httpMajor), _httpMinor(other._httpMinor) {}

std::string MessageBase::getHttpVersionString() const {
  std::stringstream res;
  res << "HTTP/" << _httpMajor;

  // only version 1.X have subversion. Starting formversion 2 subversion do not exist (HTTP/2, HTTP/3, etc)
  if (_httpMajor == 1) {
    res << "." << _httpMinor;
  }
  return res.str();
}

const std::string &MessageBase::getHeader(const std::string &key) const {
  auto it = _headers.find(key);
  if (it != _headers.end())
    return it->second;

  static std::string emptyString;
  return emptyString;
}

bool MessageBase::getHeader(const std::string &key, std::string &value) const {
  auto it = _headers.find(key);
  if (it != _headers.end()) {
    value = it->second;
    return true;
  }
  return false;
}

std::string MessageBase::GetStatusText(int status) {
#define XX(id, str)                                                                                                    \
  case id:                                                                                                             \
    return str;

  switch (status) {
    HTTP_STATUS_CODES(XX)
  default:
    // TODO: decide if needs to throw an exception
    return "Unknown";
  }
#undef XX
}

} /* namespace http */
} /* namespace native */
