#include "native/Timer.hpp"
#include "native/Error.hpp"

namespace native {

std::shared_ptr<Timer> Timer::Create(std::shared_ptr<Loop> iLoop, std::function<void()> iCallback) {
    std::shared_ptr<Timer> instance(new Timer(iLoop, std::move(iCallback)));
    instance->init();
    return instance;
}

Timer::Timer(std::shared_ptr<Loop> iLoop, std::function<void()> iCallback) : base::Handle(iLoop), _callback(iCallback) {
}

Timer::~Timer() {
    NNATIVE_ASSERT(_uvTimer.data == nullptr);
}

void Timer::init() {
    init(reinterpret_cast<uv_handle_t*>(&_uvTimer));
    int r = uv_timer_init(_loop->get(), &_uvTimer);
    NNATIVE_ASSERT(r == 0);
}

void Timer::init(uv_handle_t* iHandle) {
    base::Handle::init(iHandle);
}
std::shared_ptr<Timer> Timer::getInstanceTimer() {
    return std::static_pointer_cast<Timer>(base::Handle::getInstanceHandle());
}

Error Timer::start(const uint64_t iTimeout, const uint64_t iRepeat) {
    return Error(uv_timer_start(&_uvTimer, &Timer::OnTimeout, iTimeout, iRepeat));
}

Error Timer::stop() {
    return Error(uv_timer_stop(&_uvTimer));
}

void Timer::OnTimeout(uv_timer_t* iHandle) {
    std::shared_ptr<Timer> instance = static_cast<Timer*>(iHandle->data)->getInstanceTimer();

    if(instance->getRepeat() == 0U || !instance->_callback) {
        instance->close();
    }

    if(instance->_callback) {
        instance->_callback();
    }
}

uint64_t Timer::getRepeat() {
    return uv_timer_get_repeat(&_uvTimer);
}

void Timer::setRepeat(const uint64_t iRepeat) {
    uv_timer_set_repeat(&_uvTimer, iRepeat);
}

Error Timer::again() {
    return Error(uv_timer_again(&_uvTimer));
}

Future<std::shared_ptr<Timer>> Timer::close() {
    return base::Handle::getInstanceHandle()->close().then([](std::shared_ptr<base::Handle> iHandle) -> std::shared_ptr<Timer> {
            std::shared_ptr<Timer> instance = std::static_pointer_cast<Timer>(iHandle);

            if(instance->_onClose) {
                instance->_onClose(instance);
            }

            return instance;
    });
}

void Timer::onClose(const std::function<void(std::shared_ptr<Timer>)> iOnClose) {
    _onClose = iOnClose;
}

} /* namespace native */
