#ifndef __NATIVE_HTTP_SERVER_H__
#define __NATIVE_HTTP_SERVER_H__

#include "../net.hh"

namespace native {
namespace http {

class request;
class response;

class Server
{
protected:
    Server();
    Server(Loop &iLoop);

public:
    virtual ~Server();

    bool listen(const std::string& ip, int port, std::function<void(request&, response&)> callback);

    static std::shared_ptr<Server> Create();
    static std::shared_ptr<Server> Create(Loop &iLoop);

protected:
    std::shared_ptr<native::net::Tcp> _socket;
    std::weak_ptr<Server> _instance;
};

} // namespace http
} // namespace native

#endif // __NATIVE_HTTP_SERVER_H__
