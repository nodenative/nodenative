#include "native/Loop.hpp"
#include "native/helper/trace.hpp"

#include <memory>
#include <map>
#include <thread>

namespace {

typedef std::map<std::thread::id, std::weak_ptr<native::Loop>> LoopMapType;
LoopMapType _loopMap;

bool deregisterLoop(native::Loop *iLoopPtr) {
    for(LoopMapType::iterator it = _loopMap.begin(); it != _loopMap.end(); ++it) {
        if(!it->second.expired() && it->second.lock().get() == iLoopPtr) {
            _loopMap.erase(it);
            return true;
        }
    }

    return false;
}

void registerLoop(std::shared_ptr<native::Loop> iLoop) {
    deregisterLoop(iLoop.get());
    _loopMap[std::this_thread::get_id()] = iLoop;
}

} /* namespace */

namespace native {

void Loop::HandleDeleter::operator()(uv_loop_t* iLoop) {
    NNATIVE_DEBUG("destroying Loop.." << iLoop << ", default: " << _default);
    int res = 1;
    do {
        res = uv_loop_close(iLoop);
    } while (res != 0);

    if(!_default) {
        delete iLoop;
    }
}

std::shared_ptr<Loop> Loop::Create(bool iDefault) {
    NNATIVE_FCALL();
    static std::weak_ptr<Loop> savedPtr;

    if(iDefault && !savedPtr.expired()) {
        return savedPtr.lock();
    }

    std::shared_ptr<Loop> instance(new Loop(iDefault));
    instance->_instance = instance;

    if(iDefault) {
        savedPtr = instance;
    }

    // Register loop in the current thread
    registerLoop(instance);

    return instance;
}

std::shared_ptr<Loop> Loop::GetInstance() {
    NNATIVE_FCALL();
    return Loop::GetInstance(std::this_thread::get_id());
}

std::shared_ptr<Loop> Loop::GetInstanceOrCreateDefault() {
    NNATIVE_FCALL();
    std::shared_ptr<Loop> currLoop = Loop::GetInstance(std::this_thread::get_id());

    if(!currLoop) {
        currLoop = Loop::Create(true);
    }

    return currLoop;
}

std::shared_ptr<Loop> Loop::GetInstance(const std::thread::id &iThreadId) {
    NNATIVE_FCALL();
    LoopMapType::iterator it = _loopMap.find(iThreadId);
    if(it != _loopMap.end() && !it->second.expired()) {
        return it->second.lock();
    }
    return std::shared_ptr<Loop>();
}

bool Loop::isNotOnEventLoopThread() const {
    return (this->started() && (this->getThreadId() != std::this_thread::get_id()));
}

Loop::Loop(bool use_default) : _started(false) {
    NNATIVE_FCALL();
    if(use_default) {
        // don't delete the default Loop
        _uv_loop = std::unique_ptr<uv_loop_t, HandleDeleter>(uv_default_loop(), HandleDeleter(true));

        if(0 != uv_loop_init(_uv_loop.get())) {
            NNATIVE_DEBUG("error to init Loop " << (_uv_loop.get()));
        }
    } else {
        std::unique_ptr<uv_loop_t> loopInstance(new uv_loop_t);
        if(0 == uv_loop_init(loopInstance.get())) {
            _uv_loop = std::unique_ptr<uv_loop_t, HandleDeleter>(loopInstance.release(), HandleDeleter());
        }
    }
}

Loop::~Loop()
{
    NNATIVE_FCALL();
    deregisterLoop(this);
}

bool Loop::run() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_uv_loop);
    _started = true;
    _threadId = std::this_thread::get_id();
    registerLoop(_instance.lock());
    const bool result = (uv_run(_uv_loop.get(), UV_RUN_DEFAULT) == 0);
    _started = false;
    return result;
}

bool Loop::run_once() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_uv_loop);
    _started = true;
    _threadId = std::this_thread::get_id();
    registerLoop(_instance.lock());
    const bool result = (uv_run(_uv_loop.get(), UV_RUN_ONCE) == 0);
    _started = false;
    return result;
}

bool Loop::run_nowait()
{
    _started = true;
    _threadId = std::this_thread::get_id();
    registerLoop(_instance.lock());
    const bool result = (uv_run(_uv_loop.get(), UV_RUN_NOWAIT) == 0);
    _started = false;
    return result;
}

void Loop::update_time()
{
    uv_update_time(_uv_loop.get());
}

int64_t Loop::now()
{
    return uv_now(_uv_loop.get());
}

bool run()
{
    return Loop::Create(true)->run();
}

bool run_once()
{
    return Loop::Create(true)->run_once();
}

bool run_nowait()
{
    return Loop::Create(true)->run_nowait();
}

} /* namespace native */
