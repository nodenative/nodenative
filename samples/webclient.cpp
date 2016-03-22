#include <iostream>
#include <memory>
#include <string>
#include <native/native.hpp>
#include <fcntl.h>

using namespace native;

int main() {
    auto client = net::Tcp::Create();
    client->connect("127.0.0.1", 8080).then([client](){
        client->write("GET / HTTP/1.1\r\n\r\n").then([client](){
            std::shared_ptr<std::string> response(new std::string);
            client->readStart([client, response](const char* buf, ssize_t len){
                if(len < 0) // EOF
                {
                    std::cout << *response << std::endl;
                    client->close()
                        .then([]() {
                            std::cout << "connection closed \n";
                        }).error([](const FutureError& e){
                            std::cout << "connection error: "<< e.message() <<"\n";
                        });
                }
                else
                {
                    response->append(std::string(buf, len));
                }
            });
        }).error([](const FutureError e) {
            std::cout << "error on connect: " << e.message();
        });
    });

    return run();
}
