#include <gmock/gmock.h>
#include <native/native.hpp>

using namespace native;

namespace {

// TOOD: find a free port
int getPort() { return 8080; }

} /* namespace */

TEST(HttpServerTest, Basic)
// void deactivated()
{
  std::shared_ptr<Loop> loop = Loop::Create();
  std::shared_ptr<http::Server> server = http::Server::Create();
  std::weak_ptr<http::Server> serverWeak = server;
  int port = getPort();
  std::string bodyText("TestData body");
  bool serverClosed = false;
  bool requestSent = false;

  server->getSync("/", [bodyText, &serverClosed, &requestSent](std::shared_ptr<http::ServerConnection> connection) {
    http::ServerResponse &res = connection->getResponse();
    res.setStatus(200);
    res.setHeader("Content-Type", "text/plain");
    res.end(bodyText);
    EXPECT_EQ(requestSent, true);
    std::cout << "server response sent\n";
  });

  bool retVal = server->listen("0.0.0.0", port);

  EXPECT_EQ(retVal, true);

  EXPECT_EQ(serverClosed, false);

  std::shared_ptr<net::Tcp> client = net::Tcp::Create();
  std::weak_ptr<net::Tcp> clientWeak = client;
  client->connect("127.0.0.1", port)
      .then([clientWeak, serverWeak, bodyText, &requestSent, &serverClosed]() {
        std::shared_ptr<std::string> response(new std::string);
        requestSent = true;
        clientWeak.lock()
            ->write("GET / HTTP/1.1\r\n\r\n")
            .then([clientWeak, serverWeak, response, bodyText, &serverClosed]() {
              std::cout << "server request sent\n";
              clientWeak.lock()->readStart([clientWeak, serverWeak, response, bodyText, &serverClosed](const char *buf,
                                                                                                       ssize_t len) {
                std::cout << "server response received " << len << "\n";
                if (len < 0) {
                  // End buffer
                  std::cout << "close connection\n";
                  clientWeak.lock()
                      ->shutdown()
                      .then([clientWeak]() -> Future<void> {
                        std::cout << "client shotdown\n";
                        return clientWeak.lock()->close().then([](std::shared_ptr<base::Handle>) {});
                      })
                      .then([]() { std::cout << "client closed\n"; })
                      .error([](const FutureError &e) { std::cout << "client close error " << e.message() << "\n"; });
                  serverWeak.lock()
                      ->close()
                      .then([&serverClosed, serverWeak](std::shared_ptr<http::Server>) {
                        std::cout << "server closed " << serverWeak.use_count() << "\n";
                        serverClosed = true;
                        // server->close();
                      })
                      .error([](const FutureError &e) { std::cout << "server close error " << e.message() << "\n"; });
                  // TODO: finish
                  // EXPECT_EQ(*response, bodyText);
                } else {
                  response->append(std::string(buf, len));
                }
              });
            });
      })
      .error([](const FutureError &e) {
        std::cout << "error: " << e.message();
        EXPECT_EQ(0, 1);
      });

  NNATIVE_DEBUG("start loop");
  run();
  std::cout << "loop stoped. server count: " << server.use_count() << "\n";
  EXPECT_EQ(serverClosed, true);

  // TODO
  EXPECT_EQ(1, 1);
}
