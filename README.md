# nodenative

<b>nodenative</b> is a [C++14](http://en.wikipedia.org/wiki/C%2B%2B14) (aka C++1y) port for [node.js](https://github.com/joyent/node). This is alive project of [d5's node.native](https://github.com/d5/node.native) changed considerably for better maintenance.
<table>
  <thead>
    <tr>
      <th>Linux and OSX</th>
      <th>Coverage Status</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center">
        <a href="https://travis-ci.org/nodenative/nodenative"><img src="https://travis-ci.org/nodenative/nodenative.svg?branch=master"></a>
      </td>
      <td>
      <a href="https://coveralls.io/github/nodenative/nodenative?branch=master)"><img src="https://coveralls.io/repos/github/nodenative/nodenative/badge.svg?branch=master"></a>
      </td>
    </tr>
  </tbody>
</table>

Please note that nodenative project is <em>under heavy development</em>.

## Feature highlights
  * Basic functionality of Promise/A+ based on event pool (`native::Promise<R>`, `native::Future<R>`, `native::async(F, Args...)`). A Future callback may return a future object.
  * Thread pool (`native::worker(F, Args...)`)
  * TCP protocol (`native::net::Tcp`)
  * HTTP server integrated with `ServerPlugin` and asynchronous callbacks (`native::Future<void>`)
  * HTTP client (`native::http::get()`)
  * File System I/O (`native::fs`)
  * Timer (`native::Timer`)

## Sample code

An web-server example using asynchronous callback.
```cpp
#include <iostream>
#include <native/native.hpp>

int main() {
  std::shared_ptr<native::Loop> loop = native::Loop::Create();
  std::shared_ptr<native::http::Server> server = native::http::Server::Create(loop);
  server->get("/", [](std::shared_ptr<native::http::ServerConnection> connection) -> native::Future<void> {
    // some initial work on the main thread
    std::weak_ptr<native::http::ServerConnection> connectionWeak = connection;

    native::http::ServerResponse &res = connection->getResponse();
    res.setStatus(200);
    res.setHeader("Content-Type", "text/plain");

    // wait... I have some async work too. I will update you when I'm done.
    return native::worker([]() {
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

  server->onError([](const native::Error &err) { std::cout << "error name: " << err.name(); });

  if (!server->listen("0.0.0.0", 8080)) {
    std::cout << "cannot start server. Check the port 8080 if it is free.\n";
    return 1; // Failed to run server.
  }

  std::cout << "Server running at http://0.0.0.0:8080/" << std::endl;
  return native::run();
}
```


## Getting started

<em>nodenative</em> requires [libuv](https://github.com/libuv/libuv) and [http-parser](https://github.com/joyent/http-parser) lib to use.

### Build code

To compile included sample application(webserver.cpp) first run the following command in the project directory:
```bash
git submodule update --init
```
then generate the build files and compile:
```bash
./build.py
make -C out
```
build.py will try to download build dependencies (gyp) if missing.
If you prefer to download manually you can do:
```bash
$ git clone https://chromium.googlesource.com/external/gyp.git build/gyp
```
OR
```bash
$ svn co http://gyp.googlecode.com/svn/trunk build/gyp
```
By default will try to generate ninja file if possible, alternatively make file. After it will build in `Debug` and `Release` mode.
If you want to generate for a specific build tool use `-f <buildtool>`. e.x:
```bash
./build.py -f ninja
ninja -C out/Debug/
```
alternatively you can set custom paths to http-parser and libuv if you dont want to use the submodules.
If it is build with make in debug mode, then executables are saved to out/Debug dir.

### Build documentation

To build documentation just run the `doxygen doxyfile` command from the project root. The result document will be generated into `./out/doc/` path.

### Run samples

In samples dir you can see samples which use native library.

To run `webserver` sample compiled by make in debug mode:
```bash
out/Debug/webserver
```

### Run tests

To run `tests` compiled by make in debug mode:
```bash
out/Debug/test
```

Tested on
 - Linux with GCC 5.3.0.
 - OSX 10.10.2 with xcode7
