#include "native/async/FutureShared.hpp"
#include <memory>

namespace native {

std::shared_ptr<FutureShared<void>> FutureShared<void>::Create(std::shared_ptr<Loop> iLoop) {
    std::shared_ptr<FutureShared<void>> instance(new FutureShared<void>(iLoop));

    return instance;
}

void FutureShared<void>::setValue() {
    NNATIVE_CHECK_LOOP_THREAD(this->_loop);
    if(this->_resolver) {
        throw PromiseAlreadySatisfied();
    }

    this->_resolver = std::make_unique<FutureSharedResolverValue<void>>();

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        this->_resolver->resolve(action);
    }
}

void FutureShared<void>::setError(const FutureError& iError) {
    NNATIVE_CHECK_LOOP_THREAD(this->_loop);
    if(this->_resolver) {
        throw PromiseAlreadySatisfied();
    }

    this->_resolver = std::make_unique<FutureSharedResolverError<void>>(iError);

    for(std::shared_ptr<ActionCallbackBase<void>> action : this->_actions) {
        this->_resolver->resolve(action);
    }
}

} /* namespace native */
