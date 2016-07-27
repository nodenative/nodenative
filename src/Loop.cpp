#include "native/Loop.hpp"
#include "native/helper/trace.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>

namespace {

// TODO: move to std::shared_mutex
class RWLock {
public:
  RWLock() { uv_rwlock_init(&_lock); }
  ~RWLock() { uv_rwlock_destroy(&_lock); }
  RWLock(const RWLock &) = delete;
  RWLock operator=(const RWLock &) = delete;

  // RW
  void lock() { uv_rwlock_wrlock(&_lock); }
  bool try_lock() { return uv_rwlock_trywrlock(&_lock) == 0; }
  void unlock() { uv_rwlock_wrunlock(&_lock); }

  // R
  void lock_shared() { uv_rwlock_rdlock(&_lock); }
  bool try_lock_shared() { return uv_rwlock_tryrdlock(&_lock) == 0; }
  void unlock_shared() { uv_rwlock_rdunlock(&_lock); }

private:
  uv_rwlock_t _lock;
};

typedef std::map<std::thread::id, std::weak_ptr<native::Loop>> LoopMapType;
LoopMapType _loopMap;
RWLock _mutexloopMap;

bool deregisterLoop(native::Loop *iLoopPtr) {
  std::unique_lock<RWLock> lock(_mutexloopMap);
  for (LoopMapType::iterator it = _loopMap.begin(); it != _loopMap.end(); ++it) {
    if (!it->second.expired() && it->second.lock().get() == iLoopPtr) {
      _loopMap.erase(it);
      return true;
    }
  }

  return false;
}

void initUv() {
  static bool initialized = false;

  if (!initialized) {
    initialized = true;
    // Make inherited handles noninheritable.
    uv_disable_stdio_inheritance();
  }
}

void registerLoop(std::shared_ptr<native::Loop> iLoop) {
  deregisterLoop(iLoop.get());
  std::unique_lock<RWLock> lock(_mutexloopMap);
  _loopMap[std::this_thread::get_id()] = iLoop;
}

} /* namespace */

namespace native {

void Loop::HandleDeleter::operator()(uv_loop_t *iLoop) {
  NNATIVE_DEBUG("destroying Loop.." << iLoop << ", default: " << _default);
  int res = 1;
  do {
    res = uv_loop_close(iLoop);
  } while (res != 0);

  if (!_default) {
    delete iLoop;
  }
}

std::shared_ptr<Loop> Loop::Create() {
  NNATIVE_FCALL();
  initUv();

  std::shared_ptr<Loop> instance(new Loop());

  // Register loop in the current thread
  registerLoop(instance);

  return instance;
}

std::shared_ptr<Loop> Loop::GetInstance() {
  NNATIVE_FCALL();
  return Loop::GetInstance(std::this_thread::get_id());
}

std::shared_ptr<Loop> Loop::GetInstanceSafe() {
  NNATIVE_FCALL();
  std::shared_ptr<Loop> instance = Loop::GetInstance(std::this_thread::get_id());
  NNATIVE_ASSERT(instance);
  return instance;
}

std::shared_ptr<Loop> Loop::GetInstance(const std::thread::id &iThreadId) {
  NNATIVE_FCALL();
  std::shared_lock<RWLock> lock(_mutexloopMap);
  LoopMapType::iterator it = _loopMap.find(iThreadId);
  if (it != _loopMap.end() && !it->second.expired()) {
    return it->second.lock();
  }
  return std::shared_ptr<Loop>();
}

bool Loop::isOnEventLoopThread() const {
  return (!this->started() || (this->getThreadId() == std::this_thread::get_id()));
}

Loop::Loop() : _started(false) {
  NNATIVE_FCALL();

  // Don't use default loop because it create leaks in linux (not in osx)
  std::unique_ptr<uv_loop_t> loopInstance(new uv_loop_t);
  if (0 == uv_loop_init(loopInstance.get())) {
    _uv_loop = std::unique_ptr<uv_loop_t, HandleDeleter>(loopInstance.release(), HandleDeleter());
  }
}

Loop::~Loop() { deregisterLoop(this); }

bool Loop::run() {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_uv_loop);
  _started = true;
  _threadId = std::this_thread::get_id();
  registerLoop(getInstance());
  const bool result = (uv_run(_uv_loop.get(), UV_RUN_DEFAULT) == 0);
  _started = false;
  return result;
}

bool Loop::run_once() {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_uv_loop);
  _started = true;
  _threadId = std::this_thread::get_id();
  registerLoop(getInstance());
  const bool result = (uv_run(_uv_loop.get(), UV_RUN_ONCE) == 0);
  _started = false;
  return result;
}

bool Loop::run_nowait() {
  _started = true;
  _threadId = std::this_thread::get_id();
  registerLoop(getInstance());
  const bool result = (uv_run(_uv_loop.get(), UV_RUN_NOWAIT) == 0);
  _started = false;
  return result;
}

void Loop::update_time() { uv_update_time(_uv_loop.get()); }

uint64_t Loop::now() { return uv_now(_uv_loop.get()); }

bool run() { return Loop::GetInstanceSafe()->run(); }

bool run_once() { return Loop::GetInstanceSafe()->run_once(); }

bool run_nowait() { return Loop::GetInstanceSafe()->run_nowait(); }

} /* namespace native */
