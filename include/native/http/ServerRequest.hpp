#ifndef __NATIVE_HTTP_SERVERREQUEST_HPP__
#define __NATIVE_HTTP_SERVERREQUEST_HPP__

#include "../text.hpp"
#include "IncomingMessage.hpp"
#include "UrlObject.hpp"

#include <map>
#include <memory>

namespace native {
namespace http {

class ServerConnection;

class ServerRequest : public IncomingMessage {
  friend class ServerConnection;

private:
  ServerRequest(std::shared_ptr<ServerConnection> iTransaction);

public:
  ServerRequest() = delete;
  ~ServerRequest();

  bool isComplete() { return _complete; }

protected:
  void onMessageComplete() override;

  std::weak_ptr<ServerConnection> _connection;
  bool _complete;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_SERVERREQUEST_HPP__ */
