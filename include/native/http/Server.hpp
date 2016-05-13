#ifndef __NATIVE_HTTP_SERVER_HPP__
#define __NATIVE_HTTP_SERVER_HPP__

#include "../net.hpp"
#include "Serverplugin.hpp"
#include "Transaction.hpp"

#define NNATIVE_HTTP_METHODS(XX)                                                                                       \
  XX(get, "get")                                                                                                       \
  XX(post, "post")                                                                                                     \
  XX(put, "put")                                                                                                       \
  XX(head, "head")                                                                                                     \
  XX(deleteMethod, "delete")                                                                                           \
  XX(options, "options")                                                                                               \
  XX(trace, "trace")                                                                                                   \
  XX(copy, "copy")                                                                                                     \
  XX(lock, "lock")                                                                                                     \
  XX(mkcol, "mkcol")                                                                                                   \
  XX(move, "move")                                                                                                     \
  XX(purge, "purge")                                                                                                   \
  XX(propfind, "propfind")                                                                                             \
  XX(proppatch, "proppatch")                                                                                           \
  XX(unlock, "unlock")                                                                                                 \
  XX(report, "report")                                                                                                 \
  XX(mkactivity, "mkactivity")                                                                                         \
  XX(checkout, "checkout")                                                                                             \
  XX(merge, "merge")                                                                                                   \
  XX(mSearch, "m-search")                                                                                              \
  XX(notify, "notify")                                                                                                 \
  XX(subscribe, "subscribe")                                                                                           \
  XX(unsubscribe, "unsubscribe")                                                                                       \
  XX(patch, "patch")                                                                                                   \
  XX(search, "search")                                                                                                 \
  XX(connect, "connect")

namespace native {

namespace http {

/**
 * HTTP Server class. See example:
 * @example webserver.cpp
 */
class Server : public std::enable_shared_from_this<Server> {
  friend class Transaction;

protected:
  Server(std::shared_ptr<Loop> iLoop);

public:
  Server() = delete;
  virtual ~Server();

  bool listen(const std::string &ip, int port, std::function<void(TransactionInstance)> callback);
  Future<std::shared_ptr<Server>> close();

  static std::shared_ptr<Server> Create();
  static std::shared_ptr<Server> Create(std::shared_ptr<Loop> iLoop);
  std::shared_ptr<Server> getInstance();

  /**
   * The method to registering a ServerPlugin instance
   * @param iUri            URI template pattern
   * @param iPluginInstance Plugin instance
   */
  void use(const std::string &iUrl, std::shared_ptr<ServerPlugin> iPluginInstance);

  /**
   * The method to register a synchronous function callback
   * @param iUri            URI template pattern
   * @param iCallback           callback function to be called
   */
  void use(const std::string &iUri, std::function<void(TransactionInstance)> iCallback);

  /**
   * The method to register a asynchronous function callback
   * @param iUri            URI template pattern
   * @param iCallback           callback function to be called
   */
  void use(const std::string &iUrl, std::function<Future<void>(TransactionInstance)> iCallback);

  /**
   * Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iUri                URI template string
   * @param iCallback           callback function to be called
   */
  void method(const std::string &iUrl, std::function<void(TransactionInstance)> iCallback);

  /**
   * Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iUri                URI template string
   * @param iCallback           callback function to be called
   */
  void method(const std::string &iUri, std::function<Future<void>(TransactionInstance)> iCallback);

/**
 * For each method, exists the possibility to register a sync and async callback.
 */
#define XX(methodId, methodStr)
  void methodId(const std::string &iUri, std::function<void(TransactionInstance)> iCallback);
  void methodId(const std::string &iUri, std::function<Future<void>(TransactionInstance)> iCallback);

  NNATIVE_HTTP_METHODS(XX)
#undef XX

protected:
  std::shared_ptr<native::net::Tcp> _socket;
  std::shared_ptr<Server> _instance;
  std::shared_ptr<ServerPlugin> _defaultplugin;
  std::vector<std::shared_ptr<ServerPlugin>> _userPlugins;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVER_HPP__ */
