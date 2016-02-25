#include "native/net/Tcp.hh"

namespace native {
namespace net {

std::shared_ptr<Tcp> Tcp::Create()
{
    std::shared_ptr<Tcp> instance(new Tcp);
    instance->_instance = instance;
    return instance;
}

std::shared_ptr<Tcp> Tcp::Create(std::shared_ptr<native::Loop> iLoop)
{
    std::shared_ptr<Tcp> instance(new Tcp(iLoop));
    instance->_instance = instance;
    return instance;
}

Tcp::Tcp() :
    native::base::Stream(new uv_tcp_t)
{
    std::shared_ptr<Loop> currLoop = Loop::GetInstanceOrCreateDefault();

    uv_tcp_init(currLoop->get(), get<uv_tcp_t>());
}

Tcp::Tcp(std::shared_ptr<native::Loop> iLoop) :
    native::base::Stream(new uv_tcp_t)
{
    uv_tcp_init(iLoop->get(), get<uv_tcp_t>());
}

// TODO: bind and listen
std::shared_ptr<Tcp> Tcp::CreateServer(const std::string& ip, int port)
{
    return nullptr;
}

bool Tcp::bind(const sockaddr* iAddr, error& oError)
{
    //TODO: add flags
    oError = uv_tcp_bind(get<uv_tcp_t>(), iAddr, 0);
    return !oError;
}

bool Tcp::bind(const std::string& ip, int port, error& oError)
{
    ip4_addr addr;
    to_ip4_addr(ip.c_str(), port, addr, oError);
    if(oError) {
        return false;
    }
    return bind(reinterpret_cast<const sockaddr*>(&addr), oError);
}

bool Tcp::bind(const std::string& ip, int port)
{
    error err;
    return bind(ip, port, err);
}

bool Tcp::bind6(const std::string& ip, int port, error& oError)
{
    ip6_addr addr;
    to_ip6_addr(ip.c_str(), port, addr, oError);
    if(oError) {
        return false;
    }
    return bind(reinterpret_cast<const sockaddr*>(&addr), oError);
}

bool Tcp::bind6(const std::string& ip, int port)
{
    error err;
    return bind(ip, port, err);
}

bool Tcp::connect(const std::string& ip, int port, std::function<void(error)> callback, error& oError)
{
    callbacks::store(get()->data, native::internal::uv_cid_connect, callback);
    ip4_addr addr;
    to_ip4_addr(ip, port, addr, oError);
    if (oError)
    {
        return false;
    }
    uv_connect_t* connection_p = new uv_connect_t;
    oError = uv_tcp_connect(connection_p, get<uv_tcp_t>(), reinterpret_cast<const sockaddr*>(&addr), [](uv_connect_t* req, int status) {
        callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_connect, error(status));
        delete req;
    });
    if(oError)
    {
        delete connection_p;
    }
    return !oError;
}

bool Tcp::connect(const std::string& ip, int port, std::function<void(error)> callback)
{
    error err;
    return connect(ip, port, callback, err);
}

bool Tcp::connect6(const std::string& ip, int port, std::function<void(error)> callback, error& oError)
{
    callbacks::store(get()->data, native::internal::uv_cid_connect6, callback);
    ip6_addr addr;
    to_ip6_addr(ip, port, addr, oError);
    if (oError)
    {
        return false;
    }
    uv_connect_t* connection_p = new uv_connect_t;
    oError = uv_tcp_connect(connection_p, get<uv_tcp_t>(), reinterpret_cast<const sockaddr*>(&addr), [](uv_connect_t* req, int status) {
        callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_connect6, error(status));
        delete req;
    });
    if(oError)
    {
        delete connection_p;
    }
    return !oError;
}

bool Tcp::connect6(const std::string& ip, int port, std::function<void(error)> callback)
{
    error err;
    return connect6(ip, port, callback, err);
}

bool Tcp::getsockname(bool& ip4, std::string& ip, int& port)
{
    struct sockaddr_storage addr;
    int len = sizeof(addr);
    if(uv_tcp_getsockname(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
    {
        ip4 = (addr.ss_family == AF_INET);
        if(ip4) return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
        else return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
    }
    return false;
}

bool Tcp::getpeername(bool& ip4, std::string& ip, int& port)
{
    struct sockaddr_storage addr;
    int len = sizeof(addr);
    if(uv_tcp_getpeername(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
    {
        ip4 = (addr.ss_family == AF_INET);
        if(ip4) return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
        else return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
    }
    return false;
}

} /* namespace native */
} /* namespace net */
