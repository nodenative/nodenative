#include "native/http/MessageBase.hpp"

namespace native {
namespace http {

// Set default HTTP/1.1
// TODO: Consider to migrate to HTTP/2 by default. It requires to implement the connection negotiation first over
// HTTP/1.1
MessageBase::MessageBase() : _httpMajor(1), _httpMinor(1) {}

std::string MessageBase::getHttpVersionString() const {
  std::stringstream res;
  res << "HTTP/" << _httpMajor;

  // only version 1.X have subversion. Starting formversion 2 subversion do not exist (HTTP/2, HTTP/3, etc)
  if (_httpMajor == 1) {
    res << "." << _httpMinor;
  }
  return res.str();
}

std::string MessageBase::GetStatusText(int status) {
  switch (status) {
  case 100:
    return "Continue";
  case 101:
    return "Switching Protocols";
  case 102:
    return "Processing"; // RFC 2518, obsoleted by RFC 4918
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 202:
    return "Accepted";
  case 203:
    return "Non-Authoritative Information";
  case 204:
    return "No Content";
  case 205:
    return "Reset Content";
  case 206:
    return "Partial Content";
  case 207:
    return "Multi-Status";
  case 208:
    return "Already Reported";
  case 226:
    return "IM Used";
  case 300:
    return "Multiple Choices";
  case 301:
    return "Moved Permanently";
  case 302:
    return "Found";
  case 303:
    return "See Other";
  case 304:
    return "Not Modified";
  case 305:
    return "Use Proxy";
  // case 306: return "(reserved)";
  case 307:
    return "Temporary Redirect";
  case 308:
    return "Permanent Redirect"; // RFC 7238
  case 400:
    return "Bad Request";
  case 401:
    return "Unauthorized";
  case 402:
    return "Payment Required";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 406:
    return "Not Acceptable";
  case 407:
    return "Proxy Authentication Required";
  case 408:
    return "Request Timeout";
  case 409:
    return "Conflict";
  case 410:
    return "Gone";
  case 411:
    return "Length Required";
  case 412:
    return "Precondition Failed";
  case 413:
    return "Request Entity Too Large";
  case 414:
    return "Request-URI Too Long";
  case 415:
    return "Unsupported Media Type";
  case 416:
    return "Requested Range Not Satisfiable";
  case 417:
    return "Expectation Faiiled";
  case 418:
    return "I'm a teapot"; // RFC2324.
  case 421:
    return "Misdirected Request";
  case 422:
    return "Unprocessable Entity"; // RFC 4918
  case 423:
    return "Locked"; // RFC 4918
  case 424:
    return "Failed Dependency"; // RFC 4918
  case 425:
    return "Unordered Collection"; // RFC 4918
  case 426:
    return "Upgrade Required"; // RFC 2817
  case 428:
    return "Precondition Required"; // RFC 6585
  case 429:
    return "Too Many Requests"; // RFC 6585
  case 431:
    return "Request Header Fields Too Large"; // RFC 6585
  case 451:
    return "Unavailable For Legal Reasons";
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  case 502:
    return "Bad Gateway";
  case 503:
    return "Service Unavailable";
  case 504:
    return "Gateway Time-out";
  case 505:
    return "HTTP Version Not Supported";
  case 506:
    return "Variant Also Negotiates"; // RFC2295
  case 507:
    return "Insufficient Storage"; // RFC4918
  case 508:
    return "Loop Detected";
  case 509:
    return "Bandwidth Limit Exceeded";
  case 510:
    return "Not Extended"; // RFC2774
  case 511:
    return "Network Authentication Required"; // RFC 6585
  default:
    // TODO: decide if needs to throw an exception
    return "Unknown";
  }
}

} /* namespace http */
} /* namespace native */
