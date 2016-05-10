#include <fcntl.h>
#include <iostream>
#include <memory>
#include <native/native.hpp>
#include <string>

using namespace native;

int main() {
  std::shared_ptr<net::Tcp> client = net::Tcp::Create();
  std::weak_ptr<net::Tcp> clientWeak = client;
  client->connect("127.0.0.1", 8080).then([clientWeak]() {
    clientWeak.lock()
        ->write("GET / HTTP/1.1\r\n\r\n")
        .then([clientWeak]() {
          std::shared_ptr<std::string> response(new std::string);
          clientWeak.lock()->readStart([clientWeak, response](const char *buf, ssize_t len) {
            std::cout << "received " << len << " bytes\n";
            if (len < 0) // EOF
            {
              std::cout << *response << std::endl;
              clientWeak.lock()
                  ->close()
                  .then([](std::shared_ptr<base::Handle>) { std::cout << "connection closed \n"; })
                  .error([](const FutureError &e) { std::cout << "connection error: " << e.message() << "\n"; });
            } else {
              response->append(std::string(buf, len));
            }
          });
        })
        .error([](const FutureError e) { std::cout << "error on connect: " << e.message(); });
  });

  return run();
}
