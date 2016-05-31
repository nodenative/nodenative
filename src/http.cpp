#include "native/http.hpp"

namespace native {
namespace http {

Future<std::shared_ptr<ClientResponse>> request(std::shared_ptr<Loop> iLoop,
                                                const std::string &method,
                                                const std::string &host,
                                                const int port,
                                                const std::string &path) {
  std::shared_ptr<ClientRequest> req = ClientRequest::Create(iLoop, method, host, port, path);
  return req->end();
}

Future<std::shared_ptr<ClientResponse>> get(std::shared_ptr<Loop> iLoop, const std::string &uri) {
  std::shared_ptr<ClientRequest> req = ClientRequest::Create(iLoop, "GET", uri);
  return req->end();
}

Future<std::shared_ptr<ClientResponse>>
get(std::shared_ptr<Loop> iLoop, const std::string &host, const int port, const std::string &path) {
  std::shared_ptr<ClientRequest> req = ClientRequest::Create(iLoop, "GET", host, port, path);
  return req->end();
}

} /* namespace http */
} /* namespace native */
