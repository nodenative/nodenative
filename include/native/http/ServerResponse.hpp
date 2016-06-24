#ifndef __NATIVE_HTTP_SERVERRESPONSE_HPP__
#define __NATIVE_HTTP_SERVERRESPONSE_HPP__

#include "../text.hpp"
#include "OutgoingMessage.hpp"

#include <map>
#include <memory>
#include <string>

namespace native {

namespace http {

class ServerConnection;

class ServerResponse : public OutgoingMessage {
  friend class ServerConnection;

protected:
  ServerResponse(std::shared_ptr<ServerConnection> iTransaction);
  void setHeaderFirstLine(std::stringstream &ioMessageRaw) const override;
  virtual Future<void> sendData(const std::string &data) override;

  virtual Future<void> writeData(const std::string &str) override;
  virtual Future<void> endData(const std::string &data) override;
  void onEnd(std::function<void()> cb) { _endCbs.push_back(cb); };

public:
  ServerResponse() = delete;
  ~ServerResponse();

  void setStatus(int status_code);

  Future<void> end(const std::string &data = "") { return endData(data); }

protected:
  int _statusCode;
  std::weak_ptr<ServerConnection> _connection;
  std::list<std::function<void()>> _endCbs;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_SERVERRESPONSE_HPP__ */
