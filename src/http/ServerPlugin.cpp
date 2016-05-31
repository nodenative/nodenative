#include "native/http/ServerPlugin.hpp"
#include "native/http/Server.hpp"
#include "native/http/ServerRequest.hpp"
#include "native/http/ServerResponse.hpp"

namespace native {
namespace http {

bool CallbackDataBase::matchUri(const std::string &iUri, size_t &oLength) {
  if (!uriRegex) {
    uriRegex = Regex::Create(uri.getPattern());
  }

  std::unique_ptr<Smatch> match;

  if (uriRegex->match(iUri, match, (uriRegexMatchWholePath ? Regex::ANCHOR_START : Regex::ANCHOR_BOTH))) {
    size_t matchLength = match->length();
    if (matchLength > 0 && (matchLength == iUri.length() || iUri[matchLength] == '/')) {
      oLength = matchLength;
      return true;
    }
  }

  return false;
}

Future<void> PluginData::execute(const std::string &iUriPath, TransactionInstance iTransaction) {
  return plugin->execute(iUriPath, iTransaction);
}

ServerPlugin::ServerPlugin(std::shared_ptr<Loop> iLoop) : _loop(iLoop) {}

std::shared_ptr<ServerPlugin> ServerPlugin::getInstance() { return shared_from_this(); }

bool ServerPlugin::getCallbacksByMethod(const std::string &iMethod,
                                        const std::string &iUriPath,
                                        std::map<size_t, std::list<std::shared_ptr<CallbackDataBase>>> &oCallbacks) {
  NNATIVE_DEBUG("getCallbacksByMethod, method: " << iMethod << ", uri: " << iUriPath)
  MethodCallbackMap::iterator methodsIt = _methodCallbackMap.find(iMethod);
  bool found = false;

  if (methodsIt != _methodCallbackMap.end()) {
    for (std::shared_ptr<CallbackDataBase> callback : methodsIt->second) {
      size_t length;
      if (callback->matchUri(iUriPath, length)) {
        NNATIVE_DEBUG("found uri match, length:" << length << ", pattern: " << callback->uri.getPattern())
        found = true;
        oCallbacks[length].push_back(callback);
      }
    }
  }

  return found;
}

Future<void> ServerPlugin::execute(const std::string &iUriPath, TransactionInstance iTransaction) {
  using CallbacksIntType = std::list<std::shared_ptr<CallbackDataBase>>;
  using CallbacksMapIntType = std::map<size_t, CallbacksIntType>;
  CallbacksMapIntType callbacksMap;
  const std::string uriPath = iTransaction->getRequest().url().path();
  // Plugins
  for (std::shared_ptr<PluginData> plugin : _plugins) {
    size_t length;
    if (plugin->matchUri(iUriPath, length)) {
      callbacksMap[length].push_back(plugin);
    }
  }
  // callbacks use
  getCallbacksByMethod("", iUriPath, callbacksMap);
  // callbacks by method
  getCallbacksByMethod(iTransaction->getRequest().getMethodStr(), iUriPath, callbacksMap);
  // execute
  Future<void> result = Promise<void>::Resolve(_loop);
  bool isFirst = true;
  for (CallbacksMapIntType::value_type &callbacksIt : callbacksMap) {
    std::string newUriPath = iUriPath.substr(callbacksIt.first);
    for (std::shared_ptr<CallbackDataBase> callback : callbacksIt.second) {
      std::weak_ptr<CallbackDataBase> callbackWeak = callback;

      if (!callback) {
        continue;
      }

      if (!isFirst) {
        result = result.then([callbackWeak, iTransaction, newUriPath]() -> Future<void> {
          if (iTransaction->getResponse().isSent()) {
            return Promise<void>::Resolve(iTransaction->_server->_loop);
          }
          return callbackWeak.lock()->execute(newUriPath, iTransaction);
        });
      } else {
        isFirst = false;
        result = callback->execute(iUriPath, iTransaction);
      }
    }
  }

  return result;
}

std::shared_ptr<PluginData> ServerPlugin::addPlugin(const std::string &iUri, std::shared_ptr<ServerPlugin> iPlugin) {
  std::shared_ptr<PluginData> plugin(new PluginData(iUri, iPlugin));
  _plugins.push_back(plugin);
  return plugin;
}

bool ServerPlugin::removePlugin(std::shared_ptr<PluginData> iPlugin) {
  bool removed = false;
  _plugins.remove_if([iPlugin, &removed](std::shared_ptr<PluginData> iPluginIt) {
    if (iPlugin == iPluginIt) {
      removed = true;
      return true;
    }
    return false;
  });

  return removed;
}

bool ServerPlugin::removePlugin(std::shared_ptr<ServerPlugin> iPlugin) {
  bool removed = false;
  _plugins.remove_if([iPlugin, &removed](std::shared_ptr<PluginData> iPluginIt) {
    if (iPlugin == iPluginIt->plugin) {
      removed = true;
      return true;
    }
    return false;
  });

  return removed;
}

std::shared_ptr<CallbackData> ServerPlugin::addMethodSync(const std::string &iMethod,
                                                          const std::string &iUri,
                                                          std::function<void(TransactionInstance)> iCallback) {
  std::weak_ptr<Loop> loopInstWeak = _loop;
  return addMethod(iMethod, iUri, [iCallback, loopInstWeak](TransactionInstance iTransaction) -> Future<void> {
    iCallback(iTransaction);
    return Promise<void>::Resolve(loopInstWeak.lock());
  });
}

std::shared_ptr<CallbackData> ServerPlugin::addMethod(const std::string &iMethod,
                                                      const std::string &iUri,
                                                      std::function<Future<void>(TransactionInstance)> iCallback) {
  std::string path = iUri;
  if (path.empty()) {
    path = "/";
  }

  std::shared_ptr<CallbackData> callback(new CallbackData(path, iCallback, !iMethod.empty()));
  _methodCallbackMap[iMethod].push_back(callback);
  return callback;
}

bool ServerPlugin::removeMethod(std::shared_ptr<CallbackData> iCallback) {
  bool removed = false;

  for (MethodCallbackMap::value_type &callbacksIt : _methodCallbackMap) {
    callbacksIt.second.remove_if([iCallback, &removed](std::shared_ptr<CallbackData> iCallbackIt) {
      if (iCallback == iCallbackIt) {
        removed = true;
        return true;
      }
      return false;
    });
  }

  return removed;
}

} /* namespace http */
} /* namespace native */
