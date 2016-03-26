#include "native/http/ServerRequest.hpp"
#include "native/http/Transaction.hpp"

namespace native {
namespace http {

ServerRequest::ServerRequest(std::shared_ptr<Transaction> iTransaction) :
    _transaction(iTransaction)
{
}

ServerRequest::~ServerRequest()
{
    //printf("~ServerRequest() %x\n", this);
}

const std::string& ServerRequest::getHeader(const std::string& key) const
{
    auto it = _headers.find(key);
    if(it != _headers.end()) return it->second;
    return _defaultValue;
}

bool ServerRequest::getHeader(const std::string& key, std::string& value) const
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
