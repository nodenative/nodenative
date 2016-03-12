#include "native/http/HttpUtils.hpp"

namespace native {
namespace http {

UrlParseException::UrlParseException(const std::string& message) :
        Exception(message)
{
}

ResponseException::ResponseException(const std::string& message) :
        Exception(message)
{
}

const char* getErrorName(http_errno err)
{
    return http_errno_name(err);
}

const char* getErrorDescription(http_errno err)
{
    return http_errno_description(err);
}

const char* getMethodName(http_method m)
{
    return http_method_str(m);
}

} /* namespace http */
} /* namespace native */
