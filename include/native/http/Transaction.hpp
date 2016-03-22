#ifndef __NATIVE_HTTP_TRANSACTION_HPP__
#define __NATIVE_HTTP_TRANSACTION_HPP__

#include "../net/Tcp.hpp"

#include <http_parser.h>
#include <memory>

namespace native {

namespace http {

class Server;
class Request;
class Response;

class Transaction : public std::enable_shared_from_this<Transaction>
{
    friend class Request;
    friend class Response;
    friend class Server;
protected:
    Transaction(std::shared_ptr<Server> server);

public:
    static std::shared_ptr<Transaction> Create(std::shared_ptr<Server> iServer);
    ~Transaction();

    std::shared_ptr<Transaction> getInstance() {
        return this->shared_from_this();
    }

    Request& getRequest();
    Response& getResponse();

    Future<void> close();

private:
    bool parse(std::function<void(std::shared_ptr<Transaction>)> callback);

protected:
    virtual std::unique_ptr<Request> createRequest();
    virtual std::unique_ptr<Response> createResponse();

    http_parser _parser;
    http_parser_settings _parserSettings;
    bool _wasHeaderValue;
    std::string _lastHeaderField;
    std::string _lastHeaderValue;

    std::shared_ptr<Transaction> _instance;
    std::shared_ptr<Server> _server;
    std::shared_ptr<native::net::Tcp> _socket;
    std::unique_ptr<Request> _request;
    std::unique_ptr<Response> _response;

    std::function<void(std::shared_ptr<Transaction> iServer)> _callback;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_TRANSACTION_HPP__ */
