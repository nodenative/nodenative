#include <gmock/gmock.h>
#include <native/native.hpp>

using namespace native;

bool cbCallGet(const std::string &uriTemplate, const std::string &uri, const bool iFullMatch) {
  auto loop = Loop::Create();
  std::weak_ptr<Loop> loopWeak = loop;
  std::shared_ptr<http::Server> server = http::Server::Create();
  std::weak_ptr<http::Server> serverWeak = server;
  bool called = false;
  std::string bodyText("TestData body");

  server->getSync(uriTemplate, [&called, loopWeak, bodyText](std::shared_ptr<http::ServerConnection> connection) {
    called = true;
    http::ServerResponse &res = connection->getResponse();
    res.setStatus(200);
    res.setHeader("Content-Type", "text/plain");
    res.end(bodyText);
  });

  bool retVal = server->listen("0.0.0.0", 8080);
  EXPECT_EQ(true, retVal);

  // Send request
  http::get(loopWeak.lock(), "http://127.0.0.1:8080" + uri)
      .then([](std::shared_ptr<http::ClientResponse> res) { NNATIVE_INFO("response status:" << res->getStatusCode()); })
      .error([](const FutureError &err) {
        NNATIVE_INFO("error:" << err.message());
        // an error is not expected
        EXPECT_EQ(true, false);
      })
      .finally([serverWeak]() { serverWeak.lock()->close(); });

  loop->run();
  return called;
};

TEST(ServerPlugin, get) {

  const bool expectTrue = true;
  const bool expectFalse = false;

  EXPECT_EQ(expectTrue, cbCallGet("/", "", true));
  EXPECT_EQ(expectTrue, cbCallGet("", "/", true));
  EXPECT_EQ(expectTrue, cbCallGet("/", "/", true));
  EXPECT_EQ(expectTrue, cbCallGet("/test", "/test", true));
  EXPECT_EQ(expectTrue, cbCallGet("/test", "/test?a=true", true));
  EXPECT_EQ(expectFalse, cbCallGet("/test", "/tests", true));
}

TEST(ServerPlugin, uriTemplateValues) {
  auto loop = Loop::Create();
  std::weak_ptr<Loop> loopWeak = loop;
  std::shared_ptr<http::Server> server = http::Server::Create();
  std::weak_ptr<http::Server> serverWeak = server;
  bool calledCallback1 = false;
  bool calledCallback2 = false;
  std::string bodyText("TestData body");

  server->getSync("/:key1", [&calledCallback1, loopWeak, bodyText](std::shared_ptr<http::ServerConnection> connection) {
    calledCallback1 = true;
    http::ServerResponse &res = connection->getResponse();
    res.setStatus(200);
    res.setHeader("Content-Type", "text/plain");
    // Do not send the response to call the second callback
    std::string expectedValue("value1");
    EXPECT_EQ(connection->getRequest().getUriTemplateValues()["key1"].getString(), expectedValue);
  });

  server->getSync("/:key2", [&calledCallback2, loopWeak, bodyText](std::shared_ptr<http::ServerConnection> connection) {
    calledCallback2 = true;
    http::ServerResponse &res = connection->getResponse();
    res.end(bodyText);
    std::string expectedValue("value1");
    EXPECT_EQ(connection->getRequest().getUriTemplateValues()["key2"].getString(), expectedValue);
    EXPECT_EQ(connection->getRequest().getUriTemplateValues()["key1"].getString(), expectedValue);
  });

  bool retVal = server->listen("0.0.0.0", 8080);
  EXPECT_EQ(true, retVal);

  // Send request
  http::get(loop, "http://127.0.0.1:8080/value1")
      .then([](std::shared_ptr<http::ClientResponse> res) { NNATIVE_INFO("response status:" << res->getStatusCode()); })
      .error([](const FutureError &err) {
        NNATIVE_INFO("error:" << err.message());
        // an error is not expected
        EXPECT_EQ(true, false);
      })
      .finally([serverWeak]() { serverWeak.lock()->close(); });

  EXPECT_FALSE(calledCallback1);
  EXPECT_FALSE(calledCallback2);
  loop->run();
  EXPECT_TRUE(calledCallback1);
  EXPECT_TRUE(calledCallback2);
}
