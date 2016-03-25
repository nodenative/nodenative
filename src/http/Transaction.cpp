#include "native/http/Transaction.hpp"
#include "native/http/Request.hpp"
#include "native/http/Response.hpp"
#include "native/http/Server.hpp"

namespace native {
namespace http {

std::shared_ptr<Transaction> Transaction::Create(std::shared_ptr<Server> iServer) {
    std::shared_ptr<Transaction> instance(new Transaction(iServer));
    instance->_instance = instance;
    return instance;
}

Transaction::Transaction(std::shared_ptr<Server> iServer):
    _wasHeaderValue(true),
    _server(iServer)
{
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_server);

    _socket = native::net::Tcp::Create();
    NNATIVE_ASSERT(_server->_socket->accept(_socket));
}

Transaction::~Transaction()
{
    NNATIVE_FCALL();
    if(_socket.use_count() == 1 && _socket->isActive())
    {
        NNATIVE_DEBUG("close transaction socket from destructor");
        _socket->close();
    }
}

std::unique_ptr<Request> Transaction::createRequest() {
    return std::unique_ptr<Request>(new Request(getInstance()));
}

std::unique_ptr<Response> Transaction::createResponse() {
    return std::unique_ptr<Response>(new Response(getInstance()));
}

Request& Transaction::getRequest() {
    if(!_request) {
        _request = createRequest();
    }

    return *_request;
}

Response& Transaction::getResponse() {
    if(!_response) {
        _response = createResponse();
    }

    return *_response;
}

Future<void> Transaction::close() {
    std::weak_ptr<Transaction> transactionWeak = this->getInstance();
    return this->_socket->close()
        .then([transactionWeak]() {
            std::shared_ptr<Transaction> instance = transactionWeak.lock();
            instance->_instance.reset();
        });
}

bool Transaction::parse(std::function<void(std::shared_ptr<Transaction>)> callback)
{
    NNATIVE_FCALL();
    // Create request and response if it is not yet created
    getRequest();
    getResponse();

    http_parser_init(&_parser, HTTP_REQUEST);
    _parser.data = this;

    // store callback object
    _callback = callback;

    _parserSettings.on_message_begin = [](http_parser* parser) {
        NNATIVE_DEBUG("message begin")
        auto client = reinterpret_cast<Transaction*>(parser->data);
        client->_request = client->createRequest();
        return 0;
    };

    _parserSettings.on_url = [](http_parser* parser, const char *at, size_t len) {
        auto client = reinterpret_cast<Transaction*>(parser->data);

        //  TODO: fromBuf() can throw an exception: check
        client->_request->_url.fromBuf(at, len);

        return 0;
    };
    _parserSettings.on_header_field = [](http_parser* parser, const char* at, size_t len) {
        auto client = reinterpret_cast<Transaction*>(parser->data);

        if(client->_wasHeaderValue)
        {
            // new field started
            if(!client->_lastHeaderField.empty())
            {
                // add new entry
                client->_request->_headers[client->_lastHeaderField] = client->_lastHeaderValue;
                client->_lastHeaderValue.clear();
            }

            client->_lastHeaderField = std::string(at, len);
            client->_wasHeaderValue = false;
        }
        else
        {
            // appending
            client->_lastHeaderField += std::string(at, len);
        }
        return 0;
    };
    _parserSettings.on_header_value = [](http_parser* parser, const char* at, size_t len) {
        auto client = reinterpret_cast<Transaction*>(parser->data);

        if(!client->_wasHeaderValue)
        {
            client->_lastHeaderValue = std::string(at, len);
            client->_wasHeaderValue = true;
        }
        else
        {
            // appending
            client->_lastHeaderValue += std::string(at, len);
        }
        return 0;
    };
    _parserSettings.on_headers_complete = [](http_parser* parser) {
        auto client = reinterpret_cast<Transaction*>(parser->data);

        // add last entry if any
        if(!client->_lastHeaderField.empty()) {
            // add new entry
            client->_request->_headers[client->_lastHeaderField] = client->_lastHeaderValue;
        }

        return 0; // 1 to prevent reading of message body.
    };
    _parserSettings.on_body = [](http_parser* parser, const char* at, size_t len) {
        //printf("on_body, len of 'char* at' is %d\n", len);
        auto client = reinterpret_cast<Transaction*>(parser->data);
        client->_request->_body = std::string(at, len);
        return 0;
    };
    _parserSettings.on_message_complete = [](http_parser* parser) {
        NNATIVE_DEBUG("on_message_complete, so invoke the callback.\n");
        auto client = reinterpret_cast<Transaction*>(parser->data);
        client->_callback(client->getInstance());
        return 0; // 0 or 1?
    };

    std::weak_ptr<Transaction> instanceWeak = getInstance();

    _socket->readStart([instanceWeak](const char* buf, int len) {
        NNATIVE_ASSERT(!instanceWeak.expired());
        std::shared_ptr<Transaction> instance = instanceWeak.lock();
        //NNATIVE_DEBUG("buff [" << buf << "], len: " << len);
        // TODO: resolve multi part
        if ((buf == nullptr) || (len <= 0)) {
            instance->_response->setStatus(500);
            instance->_response->end("Invalid request format.");
        } else {
            NNATIVE_DEBUG("start http_parser_execute");
            http_parser_execute(&instance->_parser, &instance->_parserSettings, buf, len);
            NNATIVE_DEBUG("end http_parser_execute");
        }
        //instance.reset();
    });

    return true;
}

} /* namespace http */
} /* namespace native */
