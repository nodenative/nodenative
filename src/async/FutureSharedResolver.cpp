#include "native/async/FutureSharedResolver.hpp"
#include "native/async/FutureShared.hpp"

namespace native {

void FutureSharedResolverValue<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) { iFuture->resolve(); }

void FutureSharedResolverValue<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) { iFuture->resolve(); }

void FutureSharedResolverValue<void>::resolveCb(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  iFuture->resolveCb();
}

void FutureSharedResolverError<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) { iFuture->reject(_error); }

void FutureSharedResolverError<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  iFuture->reject(_error);
}

void FutureSharedResolverError<void>::resolveCb(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  iFuture->rejectCb(_error);
}

} /* namespace native */
