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

  server->getSync("/", [bodyText, &serverClosed](std::shared_ptr<http::ServerConnection> connection) {
    http::ServerResponse &res = connection->getResponse();
    res.setStatus(200);
    res.setHeader("Content-Type", "text/plain");
    res.end(bodyText);
    std::cout << "server response sent\n";
  });

  bool retVal = server->listen("0.0.0.0", port);

  EXPECT_EQ(retVal, true);

  EXPECT_EQ(serverClosed, false);

  http::get(loop, "http://127.0.0.1:8080")
      .then([](std::shared_ptr<http::ClientResponse> res) { NNATIVE_INFO("response status:" << res->getStatusCode()); })
      .error([](const FutureError &err) {
        NNATIVE_INFO("error:" << err.message());
        // an error is not expected
        EXPECT_EQ(true, false);
      })
      .finally([serverWeak, &serverClosed]() {
        serverWeak.lock()->close().then([&serverClosed](std::shared_ptr<http::Server> server) { serverClosed = true; });
      });

  NNATIVE_DEBUG("start loop");
  run();
  std::cout << "loop stoped. server count: " << server.use_count() << "\n";
  EXPECT_EQ(serverClosed, true);

  // TODO
  EXPECT_EQ(1, 1);
}
