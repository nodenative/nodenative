#include "native/async/FutureShared.h"

namespace native {

std::shared_ptr<FutureShared<void>> FutureShared<void>::Create(Loop &iLoop) {
    std::shared_ptr<FutureShared<void>> instance(new FutureShared<void>(iLoop));
    instance->setInstance(instance);

    return instance;
}

std::shared_ptr<FutureShared<void>> FutureShared<void>::Create(std::shared_ptr<uv_loop_t> iLoop) {
    std::shared_ptr<FutureShared<void>> instance(new FutureShared<void>(iLoop));
    instance->setInstance(instance);

    return instance;
}

void FutureShared<void>::setValue() {
    NNATIVE_ASSERT_MSG(isOnEventloopThread(this->_loop), "Not on the event Loop thread");
    if(this->_satisfied) {
        throw PromiseAlreadySatisfied();
    }

    this->_satisfied = true;

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetValue(action);
    }
}

void FutureShared<void>::setError(const FutureError& iError) {
    NNATIVE_ASSERT_MSG(isOnEventloopThread(this->_loop), "Not on the event Loop thread");
    if(this->_satisfied) {
        throw PromiseAlreadySatisfied();
    }

    this->_satisfied = true;
    this->_isError = true;
    this->_error = iError;

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        action->SetError(action, iError);
    }
}

void FutureShared<void>::setInstance(std::shared_ptr<FutureShared<void>> iInstance) {
    NNATIVE_ASSERT(iInstance && this == iInstance.get());
    _instance = iInstance;
}

} /* namespace native */
