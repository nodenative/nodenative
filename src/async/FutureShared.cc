#include "native/async/FutureShared.h"

namespace native {

void FutureShared<void>::setValue() {
    NNATIVE_ASSERT_MSG(isOnEventloopThread(this->_loop), "Not on the event loop thread");
    bool expected = false;
    if(!this->_satisfied.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
        throw PromiseAlreadySatisfied();
    }

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetValue(action);
    }
}

void FutureShared<void>::setError(const FutureError& iError) {
    NNATIVE_ASSERT_MSG(isOnEventloopThread(this->_loop), "Not on the event loop thread");
    bool expected = false;
    if(!this->_satisfied.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
        throw PromiseAlreadySatisfied();
    }

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetError(action, iError);
    }
}

} /* namespace native */
