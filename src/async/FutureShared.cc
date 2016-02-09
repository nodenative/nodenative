#include "native/async/FutureShared.h"

namespace native {

void FutureShared<void>::setValue() {
    if(this->_satisfied) {
        throw PromiseAlreadySatisfied();
    }

    this->_satisfied = true;

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetValue(action);
    }
}

void FutureShared<void>::setError(const FutureError& iError) {
    if(this->_satisfied) {
        throw PromiseAlreadySatisfied();
    }

    this->_satisfied = true;

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetError(action, iError);
    }
}

} /* namespace native */
