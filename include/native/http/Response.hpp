#ifndef __NATIVE_HTTP_RESPONSE_HPP__
#define __NATIVE_HTTP_RESPONSE_HPP__

#include "../text.hpp"

#include <string>
#include <map>
#include <memory>

namespace native {

namespace http {

class Transaction;

class Response
{
    friend class Transaction;

public:
    Response() = delete;
    ~Response();
    Future<void> end(const std::string& body);
    void setStatus(int status_code);
    void setHeader(const std::string& key, const std::string& value);
    static std::string getStatusText(int status);

protected:
    Response(std::shared_ptr<Transaction> iTransaction);

    std::weak_ptr<Transaction> _transaction;
    std::map<std::string, std::string, native::text::ci_less> _headers;
    int _status;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_RESPONSE_HPP__ */

