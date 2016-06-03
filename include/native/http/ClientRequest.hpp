#ifndef __NATIVE_HTTP_CLIENTREQUEST_HPP__
#define __NATIVE_HTTP_CLIENTREQUEST_HPP__

#include "../net/Tcp.hpp"
#include "../text.hpp"
#include "OutgoingMessage.hpp"

#include <map>
#include <memory>
#include <string>

namespace native {

namespace http {

class ClientResponse;

class ClientRequest : public OutgoingMessage {
  friend class ClientResponse;

protected:
  ClientRequest(std::shared_ptr<Loop> iLoop,
                const std::string &method,
                const std::string &host,
                const int &port,
                const std::string &path);

  void setHeaderFirstLine(std::stringstream &ioMessageRaw) const override;
  virtual Future<void> sendData(const std::string &data) override;
  Future<void> endData(const std::string &data) override;

public:
  ClientRequest() = delete;
  virtual ~ClientRequest();
  static std::shared_ptr<ClientRequest>
  Create(std::shared_ptr<Loop> iLoop, const std::string &method, const std::string &uri);
  static std::shared_ptr<ClientRequest> Create(std::shared_ptr<Loop> iLoop,
                                               const std::string &method,
                                               const std::string &host,
                                               const int &port,
                                               const std::string &path);

  std::shared_ptr<ClientRequest> getInstance() {
    return std::static_pointer_cast<ClientRequest>(OutgoingMessage::getInstance());
  }

  Future<std::shared_ptr<ClientResponse>> end(const std::string &data = "");

private:
  std::string _method;
  std::string _host;
  int _port;
  std::string _path;

  std::shared_ptr<Loop> _loop;
  std::shared_ptr<native::net::Tcp> _socket;
  std::shared_ptr<ClientRequest> _instance;
  std::shared_ptr<ClientResponse> _response;

  bool _connected;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_CLIENTREQUEST_HPP__ */
