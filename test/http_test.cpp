#include <native/native.hpp>
#include "gmock/gmock.h"

using namespace native;

namespace {

// TOOD: find a free port
int getPort() {
    return 8080;
}

} /* namespace */

//TEST(HttpTest, Basic)
void deactivated()
{
    std::shared_ptr<http::Server> server = http::Server::Create();
    int port = getPort();
    std::string bodyText("TestData body");
    bool serverClosed = false;
    bool requestSent = false;

    NNATIVE_DEBUG("start server");

    bool retVal = server->listen("0.0.0.0", port, [bodyText, &serverClosed, &requestSent](std::shared_ptr<http::Transaction> iTransaction) {
        NNATIVE_DEBUG("server request received");
        http::Response& res = iTransaction->getResponse();
        res.setStatus(200);
        res.setHeader("Content-Type", "text/plain");
        res.end(bodyText);
        EXPECT_EQ(requestSent, true);
    });

    EXPECT_EQ(retVal, true);

    EXPECT_EQ(serverClosed, false);

    NNATIVE_DEBUG("start client");
    std::shared_ptr<net::Tcp> client = net::Tcp::Create();
    client->connect("127.0.0.1", port).then([client, server, bodyText, &requestSent, &serverClosed]() {
        NNATIVE_DEBUG("Client connected")
        std::shared_ptr<std::string> response(new std::string);
        requestSent = true;
        client->write("GET / HTTP/1.1\r\n\r\n").then([client, server, response, bodyText, &serverClosed]() {
            client->readStart([client, server, response, bodyText, &serverClosed](const char* buf, ssize_t len) {
                NNATIVE_DEBUG("reading buff, len:" << len);
                if(len < 0) {
                    // End buffer
                    NNATIVE_DEBUG("close connections:");
                    client->close().then([](std::shared_ptr<base::Handle> iHandle){
                        NNATIVE_DEBUG("client closed");
                    }).error([](const FutureError& e){
                        NNATIVE_DEBUG("error from close client: " << e.message());
                    });
                    server->shutdown().then([server, &serverClosed]() {
                        serverClosed = true;
                        NNATIVE_DEBUG("Server shutted down");
                        //server->close();
                    }).error([](const FutureError& e){
                        NNATIVE_DEBUG("error from server shutdown: " << e.message());
                    });
                    // TODO: finish
                    //EXPECT_EQ(*response, bodyText);
                } else {
                    response->append(std::string(buf, len));
                }
            });
        });
    }).error([](const FutureError& e){
        std::cout<<"error: " << e.message();
        EXPECT_EQ(0,1);
    });

    NNATIVE_DEBUG("start loop");
    run();
    EXPECT_EQ(serverClosed, true);

    //TODO
    EXPECT_EQ(1,1);
}
