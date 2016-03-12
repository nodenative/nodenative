#include "native/http/Request.hpp"
#include "native/http/Transaction.hpp"

namespace native {
namespace http {

Request::Request(std::shared_ptr<Transaction> iTransaction) :
    _transaction(iTransaction)
{
}

Request::~Request()
{
    //printf("~Request() %x\n", this);
}

const std::string& Request::getHeader(const std::string& key) const
{
    auto it = _headers.find(key);
    if(it != _headers.end()) return it->second;
    return _defaultValue;
}

bool Request::getHeader(const std::string& key, std::string& value) const
{
    auto it = _headers.find(key);
    if(it != _headers.end())
    {
        value = it->second;
        return true;
    }
    return false;
}

} /* namespace http */
} /* namespace native */
