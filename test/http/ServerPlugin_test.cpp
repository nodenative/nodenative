#include <gmock/gmock.h>
#include <native/native.hpp>

using namespace native;

TEST(DISABLED_ServerPlugin, CallbackData) {
  auto loop = Loop::GetInstanceOrCreateDefault();
  std::weak_ptr<Loop> loopWeak = Loop::GetInstanceOrCreateDefault();

  auto cbCallGet = [loopWeak](const std::string &uriTemplate, const std::string &uri, const bool iFullMatch) -> bool {
    bool called = false;
    std::string bodyText("TestData body");
    auto cbCheck = [&called, loopWeak, bodyText](http::TransactionInstance iTransaction) -> Future<void> {
      called = true;
      http::ServerResponse &res = iTransaction->getResponse();
      res.setStatus(200);
      res.setHeader("Content-Type", "text/plain");
      res.end(bodyText);
      return Promise<void>::Resolve(loopWeak.lock());
    };

    std::shared_ptr<http::Server> server = http::Server::Create();
    std::weak_ptr<http::Server> serverWeak = server;

    server->get(uriTemplate, cbCheck);
    http::TransactionInstance transaction;
    bool retVal = server->listen("0.0.0.0", 8080);
    EXPECT_EQ(true, retVal);

    std::shared_ptr<net::Tcp> client = net::Tcp::Create();
    std::weak_ptr<net::Tcp> clientWeak = client;
    client->connect("127.0.0.1", 8080)
        .then([clientWeak, serverWeak, bodyText, uri]() {
          std::shared_ptr<std::string> response(new std::string);
          clientWeak.lock()
              ->write("GET " + uri + " HTTP/1.1\r\n\r\n")
              .then([clientWeak, serverWeak, response, bodyText]() {
                std::cout << "server request sent\n";
                clientWeak.lock()->readStart([clientWeak, serverWeak, response, bodyText](const char *buf,
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
                        .then([serverWeak](std::shared_ptr<http::Server>) {
                          std::cout << "server closed " << serverWeak.use_count() << "\n";
                        })
                        .error([](const FutureError &e) { std::cout << "server close error " << e.message() << "\n"; });
                  } else {
                    response->append(std::string(buf, len));
                  }
                });
              });
        })
        .error([serverWeak](const FutureError &e) {
          std::cout << "error: " << e.message();
          serverWeak.lock()->close();
        });

    loopWeak.lock()->run();
    return called;
  };

  EXPECT_EQ(true, cbCallGet("/", "/", true));
  EXPECT_EQ(true, cbCallGet("/test", "/test", true));
  EXPECT_EQ(false, cbCallGet("/test", "/tests", true));
}
