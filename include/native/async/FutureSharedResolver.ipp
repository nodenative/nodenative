#ifndef __NATIVE_ASYNC_FUTURESHAREDRESOLVER_IPP__
#define __NATIVE_ASYNC_FUTURESHAREDRESOLVER_IPP__

namespace native {

template<typename V>
void FutureSharedResolverValue<V>::resolve(std::shared_ptr<FutureShared<V>> iFuture) {
    iFuture->resolve(_value);
}

template<typename V>
void FutureSharedResolverValue<V>::resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) {
    iFuture->resolve(_value);
}

template<typename V>
void FutureSharedResolverValue<V>::resolveCb(std::shared_ptr<ActionCallbackBase<V>> iFuture) {
    iFuture->resolveCb(_value);
}

template<typename V>
void FutureSharedResolverError<V>::resolve(std::shared_ptr<FutureShared<V>> iFuture) {
    iFuture->reject(_error);
}

template<typename V>
void FutureSharedResolverError<V>::resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) {
    iFuture->reject(_error);
}

template<typename V>
void FutureSharedResolverError<V>::resolveCb(std::shared_ptr<ActionCallbackBase<V>> iFuture) {
    iFuture->rejectCb(_error);
}

} /* namespace native */

#endif /* __NATIVE_ASYNC_FUTURESHAREDRESOLVER_IPP__ */
