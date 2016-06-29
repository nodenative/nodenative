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

/**
 * Encode a URL to a percent-encoded form.
 * @param  url URL to encode
 * @param skipPercentSequence=false Skip percente sequence. It will check valid percent Sequense between %00 and %FF
 * @param allowedChars="/?=&#" allowed chars. This value can be change if it is required to encode a parameter value
 * @param valueAllowedChars = "-_.:[]~!$()'*,;@" value allowed chars other than alpha-numeric chars.
 * @return     encoded URL
 */
std::string encodeUrl(const std::string &url,
                      const bool skipPercentSequence = false,
                      const std::string &allowedChars = "/?=&#",
                      const std::string &valueAllowedChars = "-_.:[]~!$()'*,;@");

/**
 * Decode a URL from a percent-encoded form.
 * @param  url URL to decode
 * @param spaceAsPlus=true decode '+' char into space flag
 * @return     decoded URL
 */
std::string decodeUrl(const std::string &url, const bool spaceAsPlus = true);

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
