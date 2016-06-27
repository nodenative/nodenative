#include "native/http.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

namespace {
int hexValue(const char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }

  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }

  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }

  return -1;
}

int hexPairValue(const char c1, const char c2) {
  int v1 = hexValue(c1);
  int v2 = hexValue(c2);

  if (v1 >= 0 && v2 >= 0) {
    return (v1 << 4) + v2;
  }

  return -1;
}

} /* namespace */

namespace native {
namespace http {

std::string encodeUrl(const std::string &url, const std::string &allowedChars, const std::string &valueAllowedChars) {
  // A good example of encode/decode URL is
  // https://chromium.googlesource.com/external/libjingle/chrome-sandbox/+/master/talk/base/urlencode.cc
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;
  for (const char c : url) {
    // Keep alphanumeric and other accepted characters intact
    if (std::isalnum(c) || valueAllowedChars.find(c) != std::string::npos ||
        allowedChars.find(c) != std::string::npos) {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int((unsigned char)c);
    escaped << std::nouppercase;
  }

  return escaped.str();
}

std::string decodeUrl(const std::string &url, const bool spaceAsPlus) {
  std::string result;
  result.reserve(url.size());
  unsigned int i = 0, n = url.size();
  while (i < n) {
    std::string::value_type c = url[i];
    switch (c) {
    case '+':
      result += (spaceAsPlus ? ' ' : c);
      break;
    case '%':
      if (i + 2 < n) {
        int value = hexPairValue(url[i + 1], url[i + 2]);
        if (value >= 0) {
          result += (char)value;
          i += 2;
        } else {
          result += '?';
        }
      } else {
        result += '?';
      }
      break;
    default:
      result += c;
      break;
    }
    ++i;
  }

  return result;
}

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
