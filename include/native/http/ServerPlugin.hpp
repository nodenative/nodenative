#ifndef __NATIVE_HTTP_SERVERPLUGIN_HPP__
#define __NATIVE_HTTP_SERVERPLUGIN_HPP__

#include "../Regex.hpp"
#include "../UriTemplate.hpp"
#include "../async.hpp"
#include "ServerConnection.hpp"

#define NNATIVE_HTTP_METHODS(XX)                                                                                       \
  XX(deleteMethod, DELETE, "DELETE")                                                                                   \
  XX(get, GET, "GET")                                                                                                  \
  XX(head, HEAD, "HEAD")                                                                                               \
  XX(post, POST, "POST")                                                                                               \
  XX(put, PUT, "PUT")                                                                                                  \
  /* pathological */                                                                                                   \
  XX(connect, CONNECT, "CONNECT")                                                                                      \
  XX(options, OPTIONS, "OPTIONS")                                                                                      \
  XX(trace, TRACE, "TRACE")                                                                                            \
  /* WebDAV */                                                                                                         \
  XX(copy, COPY, "COPY")                                                                                               \
  XX(lock, LOCK, "LOCK")                                                                                               \
  XX(mkcol, MKCOL, "MKCOL")                                                                                            \
  XX(move, MOVE, "MOVE")                                                                                               \
  XX(propfind, PROPFIND, "PROPFIND")                                                                                   \
  XX(proppatch, PROPPATCH, "PROPPATCH")                                                                                \
  XX(search, SEARCH, "SEARCH")                                                                                         \
  XX(unlock, UNLOCK, "UNLOCK")                                                                                         \
  XX(bind, BIND, "BIND")                                                                                               \
  XX(rebind, REBIND, "REBIND")                                                                                         \
  XX(unbind, UNBIND, "UNBIND")                                                                                         \
  XX(acl, ACL, "ACL")                                                                                                  \
  /* subversion */                                                                                                     \
  XX(report, REPORT, "REPORT")                                                                                         \
  XX(mkactivity, MKACTIVITY, "MKACTIVITY")                                                                             \
  XX(checkout, CHECKOUT, "CHECKOUT")                                                                                   \
  XX(merge, MERGE, "MERGE")                                                                                            \
  /* upnp */                                                                                                           \
  XX(mSearch, MSEARCH, "M-SEARCH")                                                                                     \
  XX(notify, NOTIFY, "NOTIFY")                                                                                         \
  XX(subscribe, SUBSCRIBE, "SUBSCRIBE")                                                                                \
  XX(unsubscribe, UNSUBSCRIBE, "UNSUBSCRIBE")                                                                          \
  /* RFC-5789 */                                                                                                       \
  XX(patch, PATCH, "PATCH")                                                                                            \
  XX(purge, PURGE, "PURGE")                                                                                            \
  /* CalDAV */                                                                                                         \
  XX(mkcalendar, MKCALENDAR, "MKCALENDAR")                                                                             \
  /* RFC-2068, section 19.6.1.2 */                                                                                     \
  XX(link, LINK, "LINK")                                                                                               \
  XX(unlink, UNLINK, "UNLINK")

namespace native {

namespace http {

class ServerPlugin;

struct CallbackDataBase {
  CallbackDataBase() = delete;
  CallbackDataBase(const std::string &iUri, const bool iFullPathMatch)
      : uri(iUri), uriRegexMatchWholePath(iFullPathMatch){};
  virtual ~CallbackDataBase(){};
  virtual Future<void> execute(const std::string &iUriPath, std::shared_ptr<ServerConnection>) = 0;
  bool matchUri(const std::string &iRui, size_t &oLength);

  UriTemplate uri;
  std::shared_ptr<Regex> uriRegex;
  const bool uriRegexMatchWholePath;
};

struct PluginData : public CallbackDataBase {
  PluginData(const std::string &Uri, std::shared_ptr<ServerPlugin> iPluginInstance)
      : CallbackDataBase(Uri, false /*iFullPathMatch*/), plugin(iPluginInstance) {}

  Future<void> execute(const std::string &iUriPath, std::shared_ptr<ServerConnection> iTransaction) override;

  std::shared_ptr<ServerPlugin> plugin;
};

struct CallbackData : public CallbackDataBase {
  CallbackData(const std::string &iUri,
               std::function<Future<void>(std::shared_ptr<ServerConnection>)> iCallback,
               const bool iFullPathMatch)
      : CallbackDataBase(iUri, iFullPathMatch), callback(iCallback) {}

  Future<void> execute(const std::string &iUriPath, std::shared_ptr<ServerConnection> iTransaction) override {
    return callback(iTransaction);
  }

  std::function<Future<void>(std::shared_ptr<ServerConnection>)> callback;
};

class ServerPlugin : public std::enable_shared_from_this<ServerPlugin> {
public:
  ServerPlugin() = delete;
  ServerPlugin(std::shared_ptr<Loop> iLoop);
  virtual ~ServerPlugin() {}

  std::shared_ptr<ServerPlugin> getInstance();

  Future<void> execute(const std::string &iUriPath, std::shared_ptr<ServerConnection>);

  /**
   * Registering a ServerPlugin instance
   * @param iUri            URI template pattern
   * @param iPluginInstance Plugin instance
   */
  std::shared_ptr<PluginData> addPlugin(const std::string &iUri, std::shared_ptr<ServerPlugin> iPlugin);

  /**
   * Deregistering a Plugindata instance
   * @param iPluginInstance Plugin instance
   * @return true if exists, false otherwise
   */
  bool removePlugin(std::shared_ptr<PluginData> iPluginData);

  /**
   * Deregistering a ServerPlugin instance
   * @param iPluginInstance Plugin instance
   * @return true if exists, false otherwise
   */
  bool removePlugin(std::shared_ptr<ServerPlugin> iPlugin);

  /**
   * same as `registerPlugin()`
   */
  std::shared_ptr<PluginData> use(const std::string &iUri, std::shared_ptr<ServerPlugin> iPlugin) {
    return addPlugin(iUri, iPlugin);
  }

  /**
   * Add/Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iMethod method text for filtering
   * @param iUri                URI template string
   * @param iCallback           callback function to be called
   */
  std::shared_ptr<CallbackData> addMethodSync(const std::string &iMethod,
                                              const std::string &iUri,
                                              std::function<void(std::shared_ptr<ServerConnection>)> iCallback);

  /**
   * Register a syncronous method callback based on `iUri` template string.
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iMethod method text for filtering
   * @param iUri                URI template string
   * @param iCallback           callback function to be called
   */
  std::shared_ptr<CallbackData> addMethod(const std::string &iMethod,
                                          const std::string &iUri,
                                          std::function<Future<void>(std::shared_ptr<ServerConnection>)> iCallback);

  /**
   * Remove/deregister method callback
   *
   * a list of possible methods can be found https://github.com/jshttp/methods/blob/master/index.js
   *
   * @param iCallback           callback function to be called
   */
  bool removeMethod(std::shared_ptr<CallbackData> iMethodCallbackInst);

/**
 * For each method, exists the possibility to register a sync and async callback.
 */
#define XX(methodIdLower, methodIdUpper, methodStr)                                                                    \
  std::shared_ptr<CallbackData> methodIdLower##Sync(                                                                   \
      const std::string &iUri, std::function<void(std::shared_ptr<ServerConnection>)> iCallback) {                     \
    return addMethodSync(methodStr, iUri, iCallback);                                                                  \
  }                                                                                                                    \
  std::shared_ptr<CallbackData> addMethod##methodIdUpper##Sync(                                                        \
      const std::string &iUri, std::function<void(std::shared_ptr<ServerConnection>)> iCallback) {                     \
    return addMethodSync(methodStr, iUri, iCallback);                                                                  \
  }                                                                                                                    \
  std::shared_ptr<CallbackData> methodIdLower(                                                                         \
      const std::string &iUri, std::function<Future<void>(std::shared_ptr<ServerConnection>)> iCallback) {             \
    return addMethod(methodStr, iUri, iCallback);                                                                      \
  }                                                                                                                    \
  std::shared_ptr<CallbackData> addMethod##methodIdUpper(                                                              \
      const std::string &iUri, std::function<Future<void>(std::shared_ptr<ServerConnection>)> iCallback) {             \
    return addMethod(methodStr, iUri, iCallback);                                                                      \
  }                                                                                                                    \
  bool removeMethod##methodIdUpper(std::shared_ptr<CallbackData> iCallbackInst) { return removeMethod(iCallbackInst); }

  XX(use, USE, "")
  NNATIVE_HTTP_METHODS(XX)
#undef XX

  typedef std::list<std::shared_ptr<PluginData>> Plugins;
  typedef std::map<std::string, std::list<std::shared_ptr<CallbackData>>> MethodCallbackMap;

private:
  bool getCallbacksByMethod(const std::string &iMethod,
                            const std::string &iUriPath,
                            std::map<size_t, std::list<std::shared_ptr<CallbackDataBase>>> &oCallbacks);
  std::shared_ptr<Loop> _loop;
  Plugins _plugins;
  MethodCallbackMap _methodCallbackMap;
};

} // namespace http
} // namespace native

#endif /* __NATIVE_HTTP_SERVERPLUGIN_HPP__ */
