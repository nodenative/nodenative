#ifndef __NATIVE_ASYNC_FUTURESHAREDRESOLVER_HPP__
#define __NATIVE_ASYNC_FUTURESHAREDRESOLVER_HPP__

#include "./FutureError.h"

#include <memory>

namespace native {

template<typename R>
class FutureShared;

template<typename R>
class ActionCallbackBase;

template<typename V>
class FutureSharedResolver {
protected:
    FutureSharedResolver() {}
public:
    virtual ~FutureSharedResolver() {}
    
    virtual void resolve(std::shared_ptr<FutureShared<V>> iFuture) = 0;
    virtual void resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) = 0;
    virtual bool isError() = 0;
};

template<typename V>
class FutureSharedResolverValue : public FutureSharedResolver<V> {
    V _value;
public:
    FutureSharedResolverValue(V v) : _value(std::forward<V>(v)) {}
    void resolve(std::shared_ptr<FutureShared<V>> iFuture) override;
    void resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) override;
    bool isError() override { return false; }
};

template<>
class FutureSharedResolverValue<void> : public FutureSharedResolver<void> {
public:
    FutureSharedResolverValue() {}
    void resolve(std::shared_ptr<FutureShared<void>> iFuture) override;
    void resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) override;
    bool isError() override { return false; }
};

template<typename V>
class FutureSharedResolverError : public FutureSharedResolver<V> {
    FutureError _error;
public:
    FutureSharedResolverError(const FutureError &iError) : _error(iError) {}
    void resolve(std::shared_ptr<FutureShared<V>> iFuture) override;
    void resolve(std::shared_ptr<ActionCallbackBase<V>> iFuture) override;
    bool isError() override { return true; }
};

template<>
class FutureSharedResolverError<void> : public FutureSharedResolver<void> {
    FutureError _error;
public:
    FutureSharedResolverError(const FutureError &iError) : _error(iError) {}
    void resolve(std::shared_ptr<FutureShared<void>> iFuture) override;
    void resolve(std::shared_ptr<ActionCallbackBase<void>> iFuture) override;
    bool isError() override { return true; }
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_FUTURESHAREDRESOLVER_HPP__ */

