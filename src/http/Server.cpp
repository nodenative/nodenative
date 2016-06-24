#include "native/Loop.hpp"
#include "native/Timer.hpp"
#include "native/http.hpp"

namespace {

struct TimeoutEntry {
  std::weak_ptr<native::http::ServerConnection> _connectionWeak;
  uint64_t _endTime;

  TimeoutEntry() : _endTime(0) {}
  void setTime(std::shared_ptr<native::http::Server> server, bool isIdle) {
    _endTime = server->getLoop()->now() +
               (isIdle ? server->getIdleConnectionTimeoutSeconds() : server->getResponseTimeoutSeconds()) * 1000;
  }
};

struct LoopTimeoutEntries;

struct LoopTimeoutEntries {
  std::shared_ptr<native::Loop> _loop;
  std::shared_ptr<native::Timer> _timer;
  std::list<TimeoutEntry> _connectionEntries;
  static std::list<std::shared_ptr<LoopTimeoutEntries>> _loopConnectionEntries;
  bool expired() { return true; }

  void insertConnection(const TimeoutEntry &entry) {
    std::list<TimeoutEntry>::iterator it = _connectionEntries.begin();
    for (; it != _connectionEntries.end() && entry._endTime < it->_endTime; ++it) {
    }
    _connectionEntries.insert(it, entry);
  }

  static void AddConnection(std::shared_ptr<native::http::ServerConnection> connection, bool isIdle = false) {
    std::shared_ptr<native::http::Server> server = connection->getServer();
    std::shared_ptr<native::Loop> loop = server->getLoop();
    std::shared_ptr<LoopTimeoutEntries> loopEntry = FindOrCreateEntry(loop);
    TimeoutEntry entry;
    entry._connectionWeak = connection;
    entry.setTime(server, isIdle);
  }

  static void RestartTime(std::shared_ptr<native::http::ServerConnection> connection, bool isIdle = false) {
    std::shared_ptr<native::http::Server> server = connection->getServer();
    std::shared_ptr<native::Loop> loop = server->getLoop();
    std::shared_ptr<LoopTimeoutEntries> loopEntry = FindOrCreateEntry(loop);

    for (std::list<TimeoutEntry>::iterator it = loopEntry->_connectionEntries.begin();
         it != loopEntry->_connectionEntries.end(); ++it) {
      if (!it->_connectionWeak.expired() && it->_connectionWeak.lock() == connection) {
        it->setTime(server, isIdle);
        loopEntry->insertConnection(*it);
        loopEntry->_connectionEntries.erase(it);
        return;
      }
    }
  }

  static void RemoveConnection(std::shared_ptr<native::http::ServerConnection> connection) {
    std::shared_ptr<LoopTimeoutEntries> loopEntry = FindOrCreateEntry(connection->getServer()->getLoop());
    loopEntry->_connectionEntries.remove_if([connection](const TimeoutEntry &entry) {
      return entry._connectionWeak.expired() || entry._connectionWeak.lock() == connection;
    });

    if (loopEntry->_connectionEntries.size() == 0) {
      loopEntry->_timer->stop();
      _loopConnectionEntries.remove(loopEntry);
      NNATIVE_DEBUG("--_loopConnectionEntries:" << _loopConnectionEntries.size());
    }
  }

  static std::shared_ptr<LoopTimeoutEntries> FindOrCreateEntry(std::shared_ptr<native::Loop> loop);
};

std::list<std::shared_ptr<LoopTimeoutEntries>> LoopTimeoutEntries::_loopConnectionEntries;

std::shared_ptr<LoopTimeoutEntries> LoopTimeoutEntries::FindOrCreateEntry(std::shared_ptr<native::Loop> loop) {
  NNATIVE_ASSERT(loop);
  for (std::shared_ptr<LoopTimeoutEntries> loopEntry : _loopConnectionEntries) {
    if (loopEntry->_loop == loop) {
      return loopEntry;
    }
  }

  // Create a new one
  std::shared_ptr<LoopTimeoutEntries> newLoopEntry(new LoopTimeoutEntries);
  newLoopEntry->_loop = loop;

  std::weak_ptr<LoopTimeoutEntries> loopEntryWeak = newLoopEntry;

  newLoopEntry->_timer = native::Timer::Create(loop, [loopEntryWeak]() {
    if (loopEntryWeak.expired()) {
      return;
    }
    std::shared_ptr<LoopTimeoutEntries> loopEntry = loopEntryWeak.lock();
    const uint64_t timeNow = loopEntry->_loop->now();
    loopEntry->_connectionEntries.remove_if([&timeNow](const TimeoutEntry &entry) {
      if (entry._connectionWeak.expired()) {
        return true;
      } else if (timeNow >= entry._endTime) {
        std::shared_ptr<native::http::ServerConnection> connection = entry._connectionWeak.lock();
        connection->close();
        return true;
      }

      return false;
    });

    if (loopEntry->_connectionEntries.size() == 0) {
      loopEntry->_timer->stop();
      _loopConnectionEntries.remove(loopEntry);
      NNATIVE_DEBUG("--_loopConnectionEntries:" << _loopConnectionEntries.size());
    }
  });
  // repeat each second (1000ms)
  _loopConnectionEntries.push_back(newLoopEntry);
  newLoopEntry->_timer->start(1000, 1000);
  NNATIVE_DEBUG("++_loopConnectionEntries:" << _loopConnectionEntries.size());
  return newLoopEntry;
}

} /* namespace */

namespace native {
namespace http {

std::shared_ptr<Server> Server::Create() {
  std::shared_ptr<Loop> loop = Loop::GetInstanceSafe();
  std::shared_ptr<Server> instance(new Server(loop));
  instance->_instance = instance;
  return instance;
}

std::shared_ptr<Server> Server::Create(std::shared_ptr<Loop> iLoop) {
  std::shared_ptr<Server> instance(new Server(iLoop));
  instance->_instance = instance;
  return instance;
}

Server::Server(std::shared_ptr<Loop> iLoop)
    : ServerPlugin::ServerPlugin(iLoop), _loop(iLoop), _socket(native::net::Tcp::Create(iLoop)) {
  NNATIVE_FCALL();
  setIdleConnectionTimeout();
  setResponseTimeout();
}

Server::~Server() {
  NNATIVE_FCALL();
  if (_socket) {
    _socket->close();
  }
}

std::shared_ptr<Server> Server::getInstance() { return std::static_pointer_cast<Server>(ServerPlugin::getInstance()); }

void Server::onError(std::function<void(const native::Error &)> errorCb) { _errorCb = errorCb; }

void Server::error(const native::Error &e) {
  if (_errorCb) {
    _errorCb(e);
  } else {
    // unhandled error?
    throw e;
  }
}

void Server::onCreateConnection(
    std::function<std::shared_ptr<ServerConnection>(std::shared_ptr<Server>)> createConnectionCb) {
  _createConnectionCb = createConnectionCb;
}

std::shared_ptr<ServerConnection> Server::createConnection() {
  std::shared_ptr<ServerConnection> connection;
  if (_createConnectionCb) {
    connection = _createConnectionCb(getInstance());
  }

  // If callback refuze to create connection, then create a default connection
  if (!connection) {
    connection = ServerConnection::Create(getInstance());
  }

  std::weak_ptr<ServerConnection> connectionWeak = connection;
  std::weak_ptr<Server> instanceWeak = getInstance();

  _connections.push_back(connection);
  NNATIVE_DEBUG("++connections: " << _connections.size());
  connection->onClose([connectionWeak, instanceWeak]() {
    std::shared_ptr<Server> instance = instanceWeak.lock();
    std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
    instance->_connections.remove(connection);
    NNATIVE_DEBUG("--connections: " << instance->_connections.size());
    LoopTimeoutEntries::RemoveConnection(connection);
  });

  connection->onRequestReceived([connectionWeak]() {
    std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
    LoopTimeoutEntries::RestartTime(connection, false /*idle*/);
  });

  connection->onResponseSent([connectionWeak]() {
    std::shared_ptr<ServerConnection> connection = connectionWeak.lock();
    LoopTimeoutEntries::RestartTime(connection, true /*idle*/);
  });
  LoopTimeoutEntries::AddConnection(connection, true /*idle*/);

  return connection;
}

bool Server::listen(const std::string &ip, int port) {
  NNATIVE_FCALL();
  if (!_socket->bind(ip, port)) {
    NNATIVE_DEBUG("failed to bind to ip " << ip << " port " << port);
    return false;
  }

  std::weak_ptr<Server> instanceWeak = getInstance();

  if (!_socket->listen([instanceWeak](native::Error e) {
        auto instance = instanceWeak.lock();
        NNATIVE_FCALL();
        NNATIVE_DEBUG("new connection");

        if (e) {
          NNATIVE_DEBUG("error: " << e.name() << ", str:" << e.str());
          instance->error(e);
          return;
        }

        instance->createConnection()->parse();
      })) {
    NNATIVE_DEBUG("failed to listen socket");
    return false;
  }

  return true;
}

Future<std::shared_ptr<Server>> Server::close() {
  NNATIVE_DEBUG("Close server...");
  std::shared_ptr<Server> instance = getInstance();
  std::weak_ptr<Server> instanceWeak = instance;
  NNATIVE_DEBUG("closing connections:" << _connections.size());
  for (std::shared_ptr<ServerConnection> connection : _connections) {
    connection->close();
  }

  return _socket->close().then([instanceWeak](std::shared_ptr<base::Handle>) -> std::shared_ptr<Server> {
    std::shared_ptr<Server> instance = instanceWeak.lock();
    instance->_instance.reset();
    return instance;
  });
}

} /* namespace http */
} /* namespace native */
