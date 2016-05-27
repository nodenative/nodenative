#ifndef __NATIVE_ASYNC_ACTIONCALLBACKBASE_IPP__
#define __NATIVE_ASYNC_ACTIONCALLBACKBASE_IPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "./ActionCallback.hpp"
#include "./Future.hpp"

namespace native {

template <typename P> ActionCallbackBase<P>::ActionCallbackBase(std::shared_ptr<Loop> iLoop) : AsyncBase(iLoop) {}

template <typename P> std::shared_ptr<ActionCallbackBase<P>> ActionCallbackBase<P>::getInstance() {
  return std::static_pointer_cast<ActionCallbackBase<P>, AsyncBase>(AsyncBase::getInstance());
}

template <typename P> void ActionCallbackBase<P>::executeAsync() {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_resolver);
  _resolver->resolveCb(this->getInstance());
}

template <typename P> void ActionCallbackBase<P>::closeAsync() {
  NNATIVE_FCALL();
  NNATIVE_ASSERT(_resolver);
  _resolver.reset();
}

template <typename P> void ActionCallbackBase<P>::resolve(P p) {
  NNATIVE_ASSERT(!_resolver);
  _resolver = std::make_unique<FutureSharedResolverValue<P>>(std::forward<P>(p));
  this->enqueue();
}

template <typename P> void ActionCallbackBase<P>::reject(const FutureError &iError) {
  NNATIVE_ASSERT(!_resolver);
  _resolver = std::make_unique<FutureSharedResolverError<P>>(iError);
  this->enqueue();
}

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACKBASE_IPP__ */
