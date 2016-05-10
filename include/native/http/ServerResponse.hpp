#ifndef __NATIVE_HTTP_SERVERRESPONSE_HPP__
#define __NATIVE_HTTP_SERVERRESPONSE_HPP__

#include "../text.hpp"
#include "OutgoingMessage.hpp"

#include <map>
#include <memory>
#include <string>

namespace native {

namespace http {

class Transaction;

class ServerResponse : public OutgoingMessage {
  friend class Transaction;

protected:
  ServerResponse(std::shared_ptr<Transaction> iTransaction);
  virtual Future<void> send(const std::string &data) override;

public:
  ServerResponse() = delete;
  ~ServerResponse();

  Future<void> end(const std::string &data) override;

protected:
  std::weak_ptr<Transaction> _transaction;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_SERVERRESPONSE_HPP__ */
