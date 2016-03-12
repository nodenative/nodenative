#include "native/http/UrlObject.hpp"
#include "native/http/HttpUtils.hpp"

namespace native {
namespace http {

UrlObject::UrlObject() :
        _handle(),
        _buf()
{
    //printf("UrlObject() %x\n", this);
}

UrlObject::UrlObject(const UrlObject& c) :
        _handle(c._handle),
        _buf(c._buf)
{
    //printf("UrlObject(const UrlObject&) %x\n", this);
}

UrlObject& UrlObject::operator =(const UrlObject& c)
{
    //printf("UrlObject::operator =(const UrlObject&) %x\n", this);
    _handle = c._handle;
    _buf = c._buf;
    return *this;
}

UrlObject::~UrlObject()
{
    //printf("~UrlObject() %x\n", this);
}

std::string UrlObject::schema() const
{
    if(hasSchema()) return _buf.substr(_handle.field_data[UF_SCHEMA].off, _handle.field_data[UF_SCHEMA].len);
    return "HTTP";
}

std::string UrlObject::host() const
{
    // TODO: if not specified, use host name
    if(hasSchema()) return _buf.substr(_handle.field_data[UF_HOST].off, _handle.field_data[UF_HOST].len);
    return std::string("localhost");
}

int UrlObject::port() const
{
    if(hasPath()) return static_cast<int>(_handle.port);
    return (schema() == "HTTP" ? 80 : 443);
}

std::string UrlObject::path() const
{
    if(hasPath()) return _buf.substr(_handle.field_data[UF_PATH].off, _handle.field_data[UF_PATH].len);
    return std::string("/");
}

std::string UrlObject::query() const
{
    if(hasQuery()) return _buf.substr(_handle.field_data[UF_QUERY].off, _handle.field_data[UF_QUERY].len);
    return std::string();
}

std::string UrlObject::fragment() const
{
    if(hasQuery()) return _buf.substr(_handle.field_data[UF_FRAGMENT].off, _handle.field_data[UF_FRAGMENT].len);
    return std::string();
}

void UrlObject::fromBuf(const char* buf, std::size_t len, bool is_connect)
{
    // TODO: validate input parameters

    _buf = std::string(buf, len);
    if(http_parser_parse_url(buf, len, is_connect, &_handle) != 0)
    {
        // failed for some reason
        // TODO: let the caller know the Error code (or Error message)
        throw UrlParseException();
    }
}

} /* namespace http */
} /* namespace native */
