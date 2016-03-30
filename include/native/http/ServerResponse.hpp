#ifndef __NATIVE_HTTP_SERVERRESPONSE_HPP__
#define __NATIVE_HTTP_SERVERRESPONSE_HPP__

#include "OutgoingMessage.hpp"
#include "../text.hpp"

#include <string>
#include <map>
#include <memory>

namespace native {

namespace http {

class Transaction;

class ServerResponse : public OutgoingMessage {
    friend class Transaction;

protected:
    ServerResponse(std::shared_ptr<Transaction> iTransaction);
    virtual Future<void> send(const std::string& data) override;

public:
    ServerResponse() = delete;
    ~ServerResponse();

    virtual bool isRequest() const override { return false; };

    Future<void> end(const std::string &data) override;

protected:
    std::weak_ptr<Transaction> _transaction;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_SERVERRESPONSE_HPP__ */

