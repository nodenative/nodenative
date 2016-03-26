#ifndef __NATIVE_HTTP_TRANSACTION_HPP__
#define __NATIVE_HTTP_TRANSACTION_HPP__

#include "../net/Tcp.hpp"

#include <http_parser.h>
#include <memory>

namespace native {

namespace http {

class Server;
class ServerRequest;
class ServerResponse;

class Transaction : public std::enable_shared_from_this<Transaction>
{
    friend class ServerRequest;
    friend class ServerResponse;
    friend class Server;
protected:
    Transaction(std::shared_ptr<Server> server);

public:
    static std::shared_ptr<Transaction> Create(std::shared_ptr<Server> iServer);
    ~Transaction();

    std::shared_ptr<Transaction> getInstance() {
        return this->shared_from_this();
    }

    ServerRequest& getRequest();
    ServerResponse& getResponse();

    Future<void> close();

private:
    bool parse(std::function<void(std::shared_ptr<Transaction>)> callback);

protected:
    virtual std::unique_ptr<ServerRequest> createRequest();
    virtual std::unique_ptr<ServerResponse> createResponse();

    http_parser _parser;
    http_parser_settings _parserSettings;
    bool _wasHeaderValue;
    std::string _lastHeaderField;
    std::string _lastHeaderValue;

    std::shared_ptr<Transaction> _instance;
    std::shared_ptr<Server> _server;
    std::shared_ptr<native::net::Tcp> _socket;
    std::unique_ptr<ServerRequest> _request;
    std::unique_ptr<ServerResponse> _response;

    std::function<void(std::shared_ptr<Transaction> iServer)> _callback;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_TRANSACTION_HPP__ */
