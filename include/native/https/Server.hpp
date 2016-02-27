#ifndef __NATIVE_HTTPS_SERVER_HPP__
#define __NATIVE_HTTPS_SERVER_HPP__

#include "../net.hpp"
#include "../tls.hpp"

namespace native {
namespace https {

class Server : public http::Server {
protected:
    Server();
public:
    virtual ~Server();

    bool listen(const std::string& ip, int port, std::function<void(request&, response&)> callback);
}

} // namespace https
} //namespace native

#endif /* __NATIVE_HTTPS_SERVER_HPP__ */
