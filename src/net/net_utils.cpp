#include "native/net/net_utils.hpp"
#include "native/Error.hpp"

namespace native {
namespace net {

bool to_ip4_addr(const std::string& ip, int port, ip4_addr& oAddr, Error& oError) {
    oError = uv_ip4_addr(ip.c_str(), port, &oAddr);
    return !oError;
}

bool to_ip6_addr(const std::string& ip, int port, ip6_addr& oAddr, Error& oError) {
    oError = uv_ip6_addr(ip.c_str(), port, &oAddr);
    return !oError;
}

bool to_ip4_addr(const std::string& ip, int port, ip4_addr& oAddr) {
    Error err = uv_ip4_addr(ip.c_str(), port, &oAddr);;
    return err;
}

bool to_ip6_addr(const std::string& ip, int port, ip6_addr& oAddr) {
    Error err = uv_ip6_addr(ip.c_str(), port, &oAddr);
    return err;
}

bool from_ip4_addr(ip4_addr* src, std::string& ip, int& port) {
    char dest[16];
    if(uv_ip4_name(src, dest, 16) == 0) {
        ip = dest;
        port = static_cast<int>(ntohs(src->sin_port));
        return true;
    }
    return false;
}

bool from_ip6_addr(ip6_addr* src, std::string& ip, int& port) {
    char dest[46];
    if(uv_ip6_name(src, dest, 46) == 0) {
        ip = dest;
        port = static_cast<int>(ntohs(src->sin6_port));
        return true;
    }
    return false;
}

} /* namespace net */
} /* namespace native */
