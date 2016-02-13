#ifndef __NATIVE_HTTP_SERVER_H__
#define __NATIVE_HTTP_SERVER_H__

#include "../net.hh"

namespace native {
namespace http {

class request;
class response;

class Server
{
public:
    Server();
    virtual ~Server();

public:
    bool listen(const std::string& ip, int port, std::function<void(request&, response&)> callback);

private:
    std::shared_ptr<native::net::Tcp> _socket;
};

} // namespace http
} // namespace native

#endif // __NATIVE_HTTP_SERVER_H__
