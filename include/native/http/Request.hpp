#ifndef __NATIVE_HTTP_REQUEST_HPP__
#define __NATIVE_HTTP_REQUEST_HPP__

#include "UrlObject.hpp"
#include "../text.hpp"

#include <memory>
#include <map>

namespace native {
namespace http {

class Transaction;

class Request
{
    friend class Transaction;

private:
    Request(std::shared_ptr<Transaction> iTransaction);

public:
    Request() = delete;
    ~Request();
    const UrlObject& url() const { return _url; }
    const std::string& getHeader(const std::string& key) const;
    bool getHeader(const std::string& key, std::string& value) const;
    std::string getBody (void) {return _body; }

protected:
    std::weak_ptr<Transaction> _transaction;

private:
    UrlObject _url;
    std::map<std::string, std::string, native::text::ci_less> _headers;
    std::string _body;
    std::string _defaultValue;
};


} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_REQUEST_HPP__ */

