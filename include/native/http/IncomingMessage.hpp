#ifndef __NATIVE_HTTP_INCOMINGMESSAGE_HPP__
#define __NATIVE_HTTP_INCOMINGMESSAGE_HPP__

#include "../text.hpp"
#include "UrlObject.hpp"

#include <map>
#include <memory>

namespace native {
namespace http {

/**
 * Incoming HTTP message, can be request or response
 */
class IncomingMessage {
protected:
  IncomingMessage() = delete;
  IncomingMessage(const bool iIsRequest);
  virtual void onMessageComplete() = 0;

public:
  virtual ~IncomingMessage();

  void initParser();
  void parse(const char *buf, int len);

  const UrlObject &url() const { return _url; }
  const std::string &getHeader(const std::string &key) const;
  bool getHeader(const std::string &key, std::string &value) const;
  const std::string &getBody(void) const { return _body; }
  unsigned short getHttpMajor() const { return _httpMajor; }
  unsigned short getHttpMinor() const { return _httpMinor; }

  /// Method of the request message
  unsigned short getMethod() const { return _method; }

#define XX(num, name, desc)                                                                                            \
  bool isMethod##name() const { return _method == num; }
  HTTP_METHOD_MAP(XX)
#undef XX

  /// Status code of the response message
  unsigned int getStatusCode() const { return _statusCode; }

private:
  const bool _isRequest;
  unsigned short _httpMajor;
  unsigned short _httpMinor;
  unsigned int _method;
  unsigned int _statusCode;
  UrlObject _url;
  std::map<std::string, std::string, native::text::ci_less> _headers;
  std::string _body;
  std::string _defaultValue;

  http_parser _parser;
  http_parser_settings _parserSettings;
  bool _wasHeaderValue;
  std::string _lastHeaderField;
  std::string _lastHeaderValue;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_INCOMINGMESSAGE_HPP__ */
