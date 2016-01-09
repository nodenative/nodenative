#ifndef __NATIVE_HTTPS_SERVER_H__
#define __NATIVE_HTTPS_SERVER_H__

#include "net.h"
#include "tls.h"

namespace native {
namespace https {

class Server {
public:
    Server();
    virtual ~Server();

    bool listen(const std::string& ip, int port, std::function<void(request&, response&)> callback);
private:
    std::shared_ptr<native::net::tcp> socket_;
}

} // namespace https
} //namespace native

#endif
