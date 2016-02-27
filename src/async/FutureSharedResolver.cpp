#include "native/async/FutureSharedResolver.hpp"
#include "native/async/FutureShared.hpp"

namespace native {

void FutureSharedResolverValue<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) {
    iFuture->setValue();
}

void FutureSharedResolverValue<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
    iFuture->setValue();
}

void FutureSharedResolverError<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) {
    iFuture->setError(_error);
}

void FutureSharedResolverError<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
    iFuture->setError(_error);
}

} /* namespace native */