#ifndef __NATIVE_HTTP_CLIENTRESPONSE_HPP__
#define __NATIVE_HTTP_CLIENTRESPONSE_HPP__

#include "../net/Tcp.hpp"
#include "../text.hpp"
#include "IncomingMessage.hpp"
#include "UrlObject.hpp"

#include <map>
#include <memory>

namespace native {
namespace http {

class ClientRequest;

class ClientResponse : public IncomingMessage {
  friend class ClientRequest;
  ClientResponse(std::shared_ptr<ClientRequest> iReq);

public:
  ClientResponse() = delete;
  ~ClientResponse();

protected:
  void onMessageComplete() override;

private:
  std::shared_ptr<native::net::Tcp> _socket;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_CLIENTRESPONSE_HPP__ */
