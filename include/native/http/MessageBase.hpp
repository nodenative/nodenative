#ifndef __NATIVE_HTTP_MESSAGEBASE_HPP__
#define __NATIVE_HTTP_MESSAGEBASE_HPP__

#include "../text.hpp"

#include <map>
#include <memory>
#include <string>

namespace native {

namespace http {

class ClientResponse;

#define HTTP_STATUS_CODES(XX)                                                                                          \
  XX(100, "Continue")                                                                                                  \
  XX(101, "Switching Protocols")                                                                                       \
  XX(102, "Processing") /* RFC 2518, obsoleted by RFC 4918*/                                                           \
  XX(200, "OK")                                                                                                        \
  XX(201, "Created")                                                                                                   \
  XX(202, "Accepted")                                                                                                  \
  XX(203, "Non-Authoritative Information")                                                                             \
  XX(204, "No Content")                                                                                                \
  XX(205, "Reset Content")                                                                                             \
  XX(206, "Partial Content")                                                                                           \
  XX(207, "Multi-Status")                                                                                              \
  XX(208, "Already Reported")                                                                                          \
  XX(226, "IM Used")                                                                                                   \
  XX(300, "Multiple Choices")                                                                                          \
  XX(301, "Moved Permanently")                                                                                         \
  XX(302, "Found")                                                                                                     \
  XX(303, "See Other")                                                                                                 \
  XX(304, "Not Modified")                                                                                              \
  XX(305, "Use Proxy")                                                                                                 \
  XX(306, "(reserved)")                                                                                                \
  XX(307, "Temporary Redirect")                                                                                        \
  XX(308, "Permanent Redirect") /*RFC 7238*/                                                                           \
  XX(400, "Bad Request")                                                                                               \
  XX(401, "Unauthorized")                                                                                              \
  XX(402, "Payment Required")                                                                                          \
  XX(403, "Forbidden")                                                                                                 \
  XX(404, "Not Found")                                                                                                 \
  XX(405, "Method Not Allowed")                                                                                        \
  XX(406, "Not Acceptable")                                                                                            \
  XX(407, "Proxy Authentication Required")                                                                             \
  XX(408, "Request Timeout")                                                                                           \
  XX(409, "Conflict")                                                                                                  \
  XX(410, "Gone")                                                                                                      \
  XX(411, "Length Required")                                                                                           \
  XX(412, "Precondition Failed")                                                                                       \
  XX(413, "Request Entity Too Large")                                                                                  \
  XX(414, "Request-URI Too Long")                                                                                      \
  XX(415, "Unsupported Media Type")                                                                                    \
  XX(416, "Requested Range Not Satisfiable")                                                                           \
  XX(417, "Expectation Faiiled")                                                                                       \
  XX(418, "I'm a teapot") /*RFC2324.*/                                                                                 \
  XX(421, "Misdirected Request")                                                                                       \
  XX(422, "Unprocessable Entity")            /*RFC 4918*/                                                              \
  XX(423, "Locked")                          /*RFC 4918*/                                                              \
  XX(424, "Failed Dependency")               /*RFC 4918*/                                                              \
  XX(425, "Unordered Collection")            /*RFC 4918*/                                                              \
  XX(426, "Upgrade Required")                /*RFC 2817*/                                                              \
  XX(428, "Precondition Required")           /*RFC 6585*/                                                              \
  XX(429, "Too Many Requests")               /*RFC 6585*/                                                              \
  XX(431, "Request Header Fields Too Large") /*RFC 6585*/                                                              \
  XX(451, "Unavailable For Legal Reasons")                                                                             \
  XX(500, "Internal Server Error")                                                                                     \
  XX(501, "Not Implemented")                                                                                           \
  XX(502, "Bad Gateway")                                                                                               \
  XX(503, "Service Unavailable")                                                                                       \
  XX(504, "Gateway Time-out")                                                                                          \
  XX(505, "HTTP Version Not Supported")                                                                                \
  XX(506, "Variant Also Negotiates") /*RFC2295*/                                                                       \
  XX(507, "Insufficient Storage")    /*RFC4918*/                                                                       \
  XX(508, "Loop Detected")                                                                                             \
  XX(509, "Bandwidth Limit Exceeded")                                                                                  \
  XX(510, "Not Extended")                    /*RFC2774*/                                                               \
  XX(511, "Network Authentication Required") /*RFC 6585*/

class MessageBase : public std::enable_shared_from_this<MessageBase> {
protected:
  MessageBase();
  MessageBase(const MessageBase &other);
  virtual ~MessageBase() {}

public:
  std::shared_ptr<MessageBase> getInstance() { return shared_from_this(); }

  std::string getHttpVersionString() const;
  unsigned short getHttpMajor() const { return _httpMajor; }
  unsigned short getHttpMinor() const { return _httpMinor; }
  static std::string GetStatusText(int status);

protected:
  unsigned short _httpMajor;
  unsigned short _httpMinor;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_MESSAGEBASE_HPP__ */
