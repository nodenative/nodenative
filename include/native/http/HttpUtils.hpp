#ifndef __NATIVE_HTTP_HTTPUTILS_HPP__
#define __NATIVE_HTTP_HTTPUTILS_HPP__

// TODO: split into files for easy to maintain

#include <http_parser.h>
#include "../Error.hpp"

namespace native {
namespace http {

class UrlParseException : public native::Exception {
public:
    UrlParseException(const std::string& message = "Failed to parse URL.");
};

class ResponseException : public native::Exception {
public:
    ResponseException(const std::string& message = "HTTP respsonse Error.");
};

const char* getErrorName(http_errno err);
const char* getErrorDescription(http_errno err);
const char* getMethodName(http_method m);

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_HTTPUTILS_HPP__ */

