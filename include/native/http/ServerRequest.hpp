#ifndef __NATIVE_HTTP_SERVERREQUEST_HPP__
#define __NATIVE_HTTP_SERVERREQUEST_HPP__

#include "IncomingMessage.hpp"
#include "UrlObject.hpp"
#include "../text.hpp"

#include <memory>
#include <map>

namespace native {
namespace http {

class Transaction;

class ServerRequest: public IncomingMessage {
    friend class Transaction;

private:
    ServerRequest(std::shared_ptr<Transaction> iTransaction);

public:
    ServerRequest() = delete;
    ~ServerRequest();

protected:
    void initParser(std::function<void(std::shared_ptr<Transaction>)> callback);
    void onMessageComplete() override;

    std::weak_ptr<Transaction> _transaction;
    std::function<void(std::shared_ptr<Transaction> iServer)> _callback;
};


} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_SERVERREQUEST_HPP__ */

