#ifndef __NATIVE_HTTP_INCOMINGMESSAGE_HPP__
#define __NATIVE_HTTP_INCOMINGMESSAGE_HPP__

#include "../text.hpp"
#include "MessageBase.hpp"
#include "UrlObject.hpp"

#include <map>
#include <memory>

namespace native {
namespace http {

/**
 * Incoming HTTP message, can be request or response
 */
class IncomingMessage : public MessageBase {
protected:
  IncomingMessage() = delete;
  IncomingMessage(const bool iIsRequest);
  virtual void onMessageComplete() = 0;

  int parse(const char *buf, int len);
  int parse(const std::string &buf) { return parse(buf.c_str(), buf.size()); };

public:
  virtual ~IncomingMessage();

  bool isUpgrade() const { return _upgrade; }
  const UrlObject &url() const { return _url; }
  const std::string &getBody(void) const { return _body; }

  /// Method of the request message
  unsigned short getMethod() const { return _method; }
  std::string getMethodStr() const;

  bool isHeaderComplete() const { return _headerComplete; }
  bool isMessageComplete() const { return _messageComplete; }

#define XX(num, name, desc)                                                                                            \
  bool isMethod##name() const { return _method == num; }
  HTTP_METHOD_MAP(XX)
#undef XX

  /// Status code of the response message
  unsigned int getStatusCode() const { return _statusCode; }

  /**
   * Returns HTTP parser error name
   *
   * @return HTTP parser error name
   */
  std::string getErrorName() const;

  /**
   * Returns HTTP parser error description
   *
   * @return HTTP parser error description
   */
  std::string getErrorDescription() const;

private:
  void initParser();

  const bool _isRequest;
  bool _upgrade;
  bool _initiated;
  unsigned int _method;
  unsigned int _statusCode;
  std::string _responseStatusString;
  UrlObject _url;
  std::string _body;

  http_parser _parser;
  http_parser_settings _parserSettings;
  bool _wasHeaderValue;
  std::string _lastHeaderField;
  std::string _lastHeaderValue;
  bool _headerComplete;
  bool _messageComplete;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_INCOMINGMESSAGE_HPP__ */
