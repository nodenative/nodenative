# node.native 

<b>node.native</b> is a [C++14](http://en.wikipedia.org/wiki/C%2B%2B14) (aka C++1y) port for [node.js](https://github.com/joyent/node). 
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
        <a href="https://travis-ci.org/tojocky/node.native"><img src="https://travis-ci.org/tojocky/node.native.svg?branch=master"></a>
      </td>
      <td>
      <a href="https://coveralls.io/github/tojocky/node.native?branch=master)"><img src="https://coveralls.io/repos/github/tojocky/node.native/badge.svg?branch=master"></a>
      </td>
    </tr>
  </tbody>
</table>

Please note that node.native project is <em>under heavy development</em>.

## Feature highlights
 * Promises/Future functionality based on system event pool (`native::Promise<R>`, `native::Future<R>`)
 * Asynchronous callbacks based on system event pool (`native<Future<R>> native::async(F, Args...)`)
 * Thread pool based on uv_work* (`native::worker(F, Args...)`)
 * HTTP protocol

## Sample code

Simplest web-server example using node.native.
```cpp
#include <iostream>
#include <native/native.hpp>
using namespace native::http;

int main() {
    Server server;
    if(!server.listen("0.0.0.0", 8080, [](request& req, response& res) {
        res.set_status(200);
        res.set_header("Content-Type", "text/plain");
        res.end("C++ FTW\n");
    })) return 1; // Failed to run server.

    std::cout << "Server running at http://0.0.0.0:8080/" << std::endl;
    return native::run();
}
```


## Getting started

<em>node.native</em> requires [libuv](https://github.com/libuv/libuv) and [http-parser](https://github.com/joyent/http-parser) lib to use.

### Build

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
OR
$ svn co http://gyp.googlecode.com/svn/trunk build/gyp
```
by default will generate for make file. if you want to generate for a specific build tool use `-f <buildtool>`. e.x:
```bash
./build.py -f ninja
ninja -C out/Debug/
```
alternatively you can set custom paths to http-parser and libuv if you dont want to use the submodules.
If it is build with make in debug mode, then executables are saved to out/Debug dir.

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
 - Ubuntu 11.10 and GCC 4.6.1. and OSX 10.10.2
 - ArchLinux with GCC 4.9.1 and clang 3.6.0

## Other Resources

- [Mailing list](http://groups.google.com/group/nodenative)
- [Public to-to lists](https://trello.com/b/1qk3tRGS)
