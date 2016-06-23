#include <iostream>
#include <native/native.hpp>

using namespace native;
using namespace http;

int main() {
  std::shared_ptr<Loop> loop = Loop::Create();
  std::shared_ptr<Server> server = Server::Create(loop);
  server->get("/", [](std::shared_ptr<ServerConnection> connection) -> Future<void> {
    // some initial work on the main thread
    std::weak_ptr<ServerConnection> connectionWeak = connection;

    ServerResponse &res = connection->getResponse();
    res.setStatus(200);
    res.setHeader("Content-Type", "text/plain");

    // wait... I have some async work too. I will update you when I'm done.
    return worker([]() {
             // Some work on the thread pool to keep the main thread free
             std::chrono::milliseconds time(2000);
             std::this_thread::sleep_for(time);
           })
        .then([]() {
          // and some work on the main thread to sync data and avoid race condition
          std::chrono::milliseconds time(100);
          std::this_thread::sleep_for(time);
        })
        .finally([connectionWeak]() {
          // in the end send the response.
          connectionWeak.lock()->getResponse().end("C++ FTW\n");
        });
  });

  server->onError([](const Error &err) { std::cout << "error name: " << err.name(); });

  if (!server->listen("0.0.0.0", 8080)) {
    std::cout << "cannot start server. Check the port 8080 if it is free.\n";
    return 1; // Failed to run server.
  }

  std::cout << "Server running at http://0.0.0.0:8080/" << std::endl;
  return run();
}
