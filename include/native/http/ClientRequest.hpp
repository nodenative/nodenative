#ifndef __NATIVE_HTTP_CLIENTREQUEST_HPP__
#define __NATIVE_HTTP_CLIENTREQUEST_HPP__

#include "../text.hpp"
#include "OutgoingMessage.hpp"

#include <map>
#include <memory>
#include <string>

namespace native {

namespace http {

class ClientRequest : public OutgoingMessage {
  friend class Transaction;

protected:
  virtual Future<void> send(const std::string &data) override;

public:
  ClientRequest() = delete;
  ClientRequest(const std::string &host, const int &port);
  ~ClientRequest();

  Future<void> end(const std::string &data) override;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_CLIENTREQUEST_HPP__ */
