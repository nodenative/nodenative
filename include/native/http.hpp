#ifndef __NATIVE_HTTP_HPP__
#define __NATIVE_HTTP_HPP__

#include "http/ClientRequest.hpp"
#include "http/ClientResponse.hpp"
#include "http/HttpUtils.hpp"
#include "http/Server.hpp"
#include "http/ServerConnection.hpp"
#include "http/ServerPlugin.hpp"
#include "http/ServerRequest.hpp"
#include "http/ServerResponse.hpp"
#include "http/UrlObject.hpp"

namespace native {
namespace http {

Future<std::shared_ptr<ClientResponse>> request(std::shared_ptr<Loop> iLoop,
                                                const std::string &method,
                                                const std::string &host,
                                                const int port,
                                                const std::string &path);

Future<std::shared_ptr<ClientResponse>> get(std::shared_ptr<Loop> iLoop, const std::string &uri);
Future<std::shared_ptr<ClientResponse>>
get(std::shared_ptr<Loop> iLoop, const std::string &host, const int port, const std::string &path);

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_HPP__ */
