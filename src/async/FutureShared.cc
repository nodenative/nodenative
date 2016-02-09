#include "native/async/FutureShared.h"

namespace native {

void FutureShared<void>::setValue() {
    bool expected = false;
    if(!this->_satisfied.compare_exchange_strong(expected, true)) {
        throw PromiseAlreadySatisfied();
    }

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetValue(action);
    }
}

void FutureShared<void>::setError(const FutureError& iError) {
    bool expected = false;
    if(!this->_satisfied.compare_exchange_strong(expected, true)) {
        throw PromiseAlreadySatisfied();
    }

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetError(action, iError);
    }
}

} /* namespace native */
