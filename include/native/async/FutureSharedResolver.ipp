#ifndef __NATIVE_ASYNC_FUTURESHAREDRESOLVER_IPP__
#define __NATIVE_ASYNC_FUTURESHAREDRESOLVER_IPP__

namespace native {

template<typename V>
void FutureSharedResolverValue<V>::resolve(std::shared_ptr<FutureShared<V>> iFuture) {
    iFuture->setValue(_value);
}

template<typename V>
void FutureSharedResolverValue<V>::resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) {
    iFuture->setValue(_value);
}

template<typename V>
void FutureSharedResolverError<V>::resolve(std::shared_ptr<FutureShared<V>> iFuture) {
    iFuture->setError(_error);
}

template<typename V>
void FutureSharedResolverError<V>::resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) {
    iFuture->setError(_error);
}

} /* namespace native */

#endif /* __NATIVE_ASYNC_FUTURESHAREDRESOLVER_IPP__ */