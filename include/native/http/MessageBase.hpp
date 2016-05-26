#ifndef __NATIVE_HTTP_MESSAGEBASE_HPP__
#define __NATIVE_HTTP_MESSAGEBASE_HPP__

#include "../text.hpp"

#include <map>
#include <memory>
#include <string>

namespace native {

namespace http {

class ClientResponse;

class MessageBase : public std::enable_shared_from_this<MessageBase> {
protected:
  MessageBase();
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
