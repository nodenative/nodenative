#include "native/http.h"

namespace native {
namespace http {

std::shared_ptr<Server> Server::Create()
{
    std::shared_ptr<Server> instance(new Server());
    instance->_instance = instance;
    return instance;
}

Server::Server() : _socket(new native::net::Tcp)
{
}

Server::~Server()
{
    if(_socket)
    {
        _socket->close([](){});
    }
}

bool Server::listen(const std::string& ip, int port, std::function<void(request&, response&)> callback)
{
    if(!_socket->bind(ip, port)) {
        return false;
    }

    if(!_socket->listen([=](native::error e) {
        if(e)
        {
            // TODO: handle client connection error
        }
        else
        {
            auto client = new client_context(_socket.get());
            client->parse(callback);
        }
    })) return false;

    return true;
}

} /* namespace http */
} /* namespace native */
