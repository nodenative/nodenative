#include "native/async/FutureSharedResolver.hpp"
#include "native/async.hpp"
#include "native/async.ipp"

namespace native {

void FutureSharedResolverValue<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) { iFuture->resolve(); }

void FutureSharedResolverValue<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) { iFuture->resolve(); }

void FutureSharedResolverValue<void>::resolveCb(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  iFuture->resolveCb();
}

FutureSharedResolverFuture<void>::FutureSharedResolverFuture(Future<void> iFuture)
    : _future(iFuture.getFutureShared()) {}

void FutureSharedResolverFuture<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) {
  _future->then([iFuture]() { iFuture->resolve(); })->error([iFuture](const FutureError &err) {
    iFuture->reject(err);
  });
}

void FutureSharedResolverFuture<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  _future->then([iFuture]() { iFuture->resolve(); })->error([iFuture](const FutureError &err) {
    iFuture->reject(err);
  });
}

void FutureSharedResolverFuture<void>::resolveCb(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  _future->then([iFuture]() { iFuture->resolveCb(); })->error([iFuture](const FutureError &err) {
    iFuture->rejectCb(err);
  });
}

void FutureSharedResolverError<void>::resolve(std::shared_ptr<FutureShared<void>> iFuture) { iFuture->reject(_error); }

void FutureSharedResolverError<void>::resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  iFuture->reject(_error);
}

void FutureSharedResolverError<void>::resolveCb(std::shared_ptr<ActionCallbackBase<void>> iFuture) {
  iFuture->rejectCb(_error);
}

} /* namespace native */
