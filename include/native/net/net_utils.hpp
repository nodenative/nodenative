#ifndef __NATIVE_NET_NETUTILS_HPP__
#define __NATIVE_NET_NETUTILS_HPP__

#include "../base.hpp"

namespace native {
class error;

namespace net {
typedef sockaddr_in ip4_addr;
typedef sockaddr_in6 ip6_addr;

bool to_ip4_addr(const std::string &ip, int port, ip4_addr &oAddr, Error &oError);
bool to_ip6_addr(const std::string &ip, int port, ip6_addr &oAddr, Error &oError);

bool to_ip4_addr(const std::string &ip, int port, ip4_addr &oAddr);
bool to_ip6_addr(const std::string &ip, int port, ip6_addr &oAddr);

bool from_ip4_addr(ip4_addr *src, std::string &ip, int &port);
bool from_ip6_addr(ip6_addr *src, std::string &ip, int &port);
}
}

#endif /* __NATIVE_NET_NETUTILS_HPP__ */
