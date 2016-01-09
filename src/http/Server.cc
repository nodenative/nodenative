#include "native/http.h"

using namespace native;
using namespace http;

Server::Server() : socket_(new native::net::tcp)
{
}

Server::~Server()
{
    if(socket_)
    {
        socket_->close([](){});
    }
}

bool Server::listen(const std::string& ip, int port, std::function<void(request&, response&)> callback)
{
    if(!socket_->bind(ip, port)) {
        return false;
    }

    if(!socket_->listen([=](native::error e) {
        if(e)
        {
            // TODO: handle client connection error
        }
        else
        {
            auto client = new client_context(socket_.get());
            client->parse(callback);
        }
    })) return false;

    return true;
}
