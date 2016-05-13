# nodenative

<b>nodenative</b> is a [C++14](http://en.wikipedia.org/wiki/C%2B%2B14) (aka C++1y) port for [node.js](https://github.com/joyent/node).
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
 * Basic functionality of Promise/A+ based on event pool (`native::Promise<R>`, `native::Future<R>`, `native<Future<R>> native::async(F, Args...)`)
 * Thread pool based on uv_work* (`native::worker(F, Args...)`)
 * HTTP protocol
 * File System I/O

## Sample code

Simplest web-server example using nodenative.
```cpp
#include <iostream>
#include <native/native.hpp>
using namespace native::http;

int main() {
    Server server;
    if(!server.listen("0.0.0.0", 8080, [](http::shared_ptr<Transaction> iTransaction) {
        ServerResponse& res = iTransaction->getResponse();
        res.setStatus(200);
        res.setHeader("Content-Type", "text/plain");
        res.end("C++ FTW\n");
    })) return 1; // Failed to run server.

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
