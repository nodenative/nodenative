#include <gmock/gmock.h>
#include <native/native.hpp>

using namespace native;

TEST(ServerPlugin, get) {
  auto loop = Loop::Create();
  std::weak_ptr<Loop> loopWeak = Loop::GetInstanceSafe();

  auto cbCallGet = [loopWeak](const std::string &uriTemplate, const std::string &uri, const bool iFullMatch) -> bool {
    std::shared_ptr<http::Server> server = http::Server::Create();
    std::weak_ptr<http::Server> serverWeak = server;
    bool called = false;
    std::string bodyText("TestData body");

    server->getSync(uriTemplate, [&called, loopWeak, bodyText](http::TransactionInstance iTransaction) {
      called = true;
      http::ServerResponse &res = iTransaction->getResponse();
      res.setStatus(200);
      res.setHeader("Content-Type", "text/plain");
      res.end(bodyText);
    });

    bool retVal = server->listen("0.0.0.0", 8080);
    EXPECT_EQ(true, retVal);

    // Send request
    http::get(loopWeak.lock(), "http://127.0.0.1:8080" + uri)
        .then(
            [](std::shared_ptr<http::ClientResponse> res) { NNATIVE_INFO("response status:" << res->getStatusCode()); })
        .error([](const FutureError &err) {
          NNATIVE_INFO("error:" << err.message());
          // an error is not expected
          EXPECT_EQ(true, false);
        })
        .finally([serverWeak]() { serverWeak.lock()->close(); });

    loopWeak.lock()->run();
    return called;
  };

  const bool expectTrue = true;
  const bool expectFalse = false;

  EXPECT_EQ(expectTrue, cbCallGet("/", "", true));
  EXPECT_EQ(expectTrue, cbCallGet("", "/", true));
  EXPECT_EQ(expectTrue, cbCallGet("/", "/", true));
  EXPECT_EQ(expectTrue, cbCallGet("/test", "/test", true));
  EXPECT_EQ(expectTrue, cbCallGet("/test", "/test?a=true", true));
  EXPECT_EQ(expectFalse, cbCallGet("/test", "/tests", true));
}
