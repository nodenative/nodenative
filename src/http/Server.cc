#include "native/http.h"
#include "native/Loop.hh"

namespace native {
namespace http {

std::shared_ptr<Server> Server::Create()
{
    std::shared_ptr<Server> instance(new Server());
    instance->_instance = instance;
    return instance;
}

std::shared_ptr<Server> Server::Create(std::shared_ptr<Loop> iLoop)
{
    std::shared_ptr<Server> instance(new Server(iLoop));
    instance->_instance = instance;
    return instance;
}

Server::Server(std::shared_ptr<Loop> iLoop) : _socket(native::net::Tcp::Create(iLoop))
{
}

Server::Server() : _socket(native::net::Tcp::Create())
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
            auto client = new client_context(_socket);
            client->parse(callback);
        }
    })) return false;

    return true;
}

} /* namespace http */
} /* namespace native */
