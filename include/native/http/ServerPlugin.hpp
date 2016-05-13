#ifndef __NATIVE_HTTP_SERVERPLUGIN_HPP__
#define __NATIVE_HTTP_SERVERPLUGIN_HPP__

#include "../UriTemplate.hpp"
#include "../async.hpp"
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

class ServerPlugin : public std::enable_shared_from_this<ServerPlugin> {
public:
  struct MethodCallcback {
    MethodCallcback(std::string &Uri, std::function<Future<void>(TransactionInstance)> iCallback)
        : uriTemplate(Uri), callback(iCallback) {}

    UriTemplate uriTemplate;
    std::function<Future<void>(TransactionInstance)> callback;
  };

  virtual ~ServerPlugin() {}

  std::shared_ptr<ServerPlugin> getInstance();

  /**
   * Registering a ServerPlugin instance
   * @param iUri            URI template pattern
   * @param iPluginInstance Plugin instance
   */
  std::shared_ptr<ServerPlugin> registerPlugin(const std::string &iUrl, std::shared_ptr<ServerPlugin> iPluginInstance);

  /**
   * Deregistering a ServerPlugin instance
   * @param iPluginInstance Plugin instance
   */
  void deregisterPlugin(std::shared_ptr<ServerPlugin> iPluginInst);

  /**
   * same as `registerPlugin()`
   */
  std::shared_ptr<ServerPlugin> use(const std::string &iUrl, std::shared_ptr<ServerPlugin> iPluginInstance) {
    return registerPlugin(iUrl, iPluginInstance);
  }

  /**
   * Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iMethod method text for filtering
   * @param iUri                URI template string
   * @param iCallback           callback function to be called
   */
  std::shared_ptr<MethodCallcback> registerMethod(const std::string &iMethod,
                                                  const std::string &iUrl,
                                                  std::function<void(TransactionInstance)> iCallback);

  /**
   * Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iMethod method text for filtering
   * @param iUri                URI template string
   * @param iCallback           callback function to be called
   */
  std::shared_ptr<MethodCallcback> registerMethod(const std::string &iMethod,
                                                  const std::string &iUri,
                                                  std::function<Future<void>(TransactionInstance)> iCallback);

  /**
   * Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iMethod method text for filtering
   * @param iCallback           callback function to be called
   */
  void deregisterMethod(const std::string &iMethod, std::shared_ptr<MethodCallcback> iMethodCallbackInst);

/**
 * For each method, exists the possibility to register a sync and async callback.
 */
#define XX(methodId, methodStr)                                                                                        \
  std::shared_ptr<MethodCallcback> methodId(const std::string &iUri,                                                   \
                                            std::function<void(TransactionInstance)> iCallback) {                      \
    return registerMethod(methodStr, iUri, iCallback);                                                                 \
  }                                                                                                                    \
  std::shared_ptr<MethodCallcback> methodId(const std::string &iUri,                                                   \
                                            std::function<Future<void>(TransactionInstance)> iCallback) {              \
    return registerMethod(methodStr, iUri, iCallback);                                                                 \
  }                                                                                                                    \
  void methodId##Deregister(std::shared_ptr<MethodCallcback> iMethodCallbackInst) {                                    \
    deregisterMethod(methodStr, iMethodCallbackInst);                                                                  \
  }

  XX(use, "")
  NNATIVE_HTTP_METHODS(XX)
#undef XX

private:
  std::list<std::shared_ptr<ServerPlugin>> _userPlugins;
  std::map<std::string, std::list<std::shared_ptr<MethodCallcback>>> _methodCallbackMap;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVERPLUGIN_HPP__ */
