#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_H__
#define __NATIVE_ASYNC_ACTIONCALLBACK_H__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.h"
#include "FutureError.h"
#include "AsyncBase.h"

namespace native {

template<class R>
class FutureShared;

template<typename R>
class Future;

/** Action callback base class template
 */
template<typename P>
class ActionCallbackBase {
protected:
    std::weak_ptr<ActionCallbackBase<P>> _instance;

public:
    virtual ~ActionCallbackBase() {}

    static void SetValue(std::shared_ptr<ActionCallbackBase<P>> iInstance, P p);
    static void SetError(std::shared_ptr<ActionCallbackBase<P>> iInstance, const FutureError&);

    virtual void setValueCb(P) = 0;
    virtual void setErrorCb(const FutureError&) = 0;
    virtual std::shared_ptr<Loop> getLoop() = 0;
};

/** Acction callback base class template specialization for void type.
 */
template<>
class ActionCallbackBase<void> {
protected:
    std::weak_ptr<ActionCallbackBase<void>> _instance;

public:
    virtual ~ActionCallbackBase() {}

    static void SetValue(std::shared_ptr<ActionCallbackBase<void>> iInstance);
    static void SetError(std::shared_ptr<ActionCallbackBase<void>> iInstance, const FutureError &iError);

    virtual void setValueCb() = 0;
    virtual void setErrorCb(const FutureError&) = 0;
    virtual std::shared_ptr<Loop> getLoop() = 0;
};

template<typename P>
class ActionCallbackBaseDetached : public AsyncBase {
private:
    std::shared_ptr<ActionCallbackBase<P>> _instance;
    std::tuple<P> _args;

public:
    ActionCallbackBaseDetached() = delete;
    ActionCallbackBaseDetached(std::shared_ptr<ActionCallbackBase<P>> iInstance, P p);
    ~ActionCallbackBaseDetached() {
        NNATIVE_FCALL();
    }

    void executeAsync() override;

    static void Enqueue(std::shared_ptr<ActionCallbackBase<P>> iInstance, P p);
};

template<>
class ActionCallbackBaseDetached<void> : public AsyncBase {
private:
    std::shared_ptr<ActionCallbackBase<void>> _instance;

public:
    ActionCallbackBaseDetached() = delete;
    ActionCallbackBaseDetached(std::shared_ptr<ActionCallbackBase<void>> iInstance) : AsyncBase(iInstance->getLoop()), _instance(iInstance) {}
    ~ActionCallbackBaseDetached() {
        NNATIVE_FCALL();
    }

    void executeAsync() override;

    static void Enqueue(std::shared_ptr<ActionCallbackBase<void>> iInstance);
};

template<typename P>
class ActionCallbackBaseDetachedError : public AsyncBase {
private:
    std::shared_ptr<ActionCallbackBase<P>> _instance;
    FutureError _error;

public:
    ActionCallbackBaseDetachedError() = delete;
    ActionCallbackBaseDetachedError(std::shared_ptr<ActionCallbackBase<P>> iInstance, const FutureError &iError);
    ~ActionCallbackBaseDetachedError() {
        NNATIVE_FCALL();
    }
    void executeAsync() override;

    static void Enqueue(std::shared_ptr<ActionCallbackBase<P>> iInstance, const FutureError &iError);
};

template<typename R, typename... Args>
class ActionCallback: public ActionCallbackBase<void> {
    std::function<R(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef R ResultType;

    ActionCallback() = delete;
    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

template<typename R, typename... Args>
class ActionCallback<Future<R>, Args...>: public ActionCallbackBase<void> {
    std::function<Future<R>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef R ResultType;

    ActionCallback() = delete;
    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

template<typename... Args>
class ActionCallback<Future<void>, Args...>: public ActionCallbackBase<void> {
    std::function<Future<void>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef void ResultType;

    ActionCallback() = delete;
    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

template<typename... Args>
class ActionCallback<void, Args...>: public ActionCallbackBase<void> {
    std::function<void(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef void ResultType;

    ActionCallback() = delete;
    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

/** Value resolver callback class template. It call the function callback in case if the future resolved
 */
template<typename R, typename P, typename... Args>
class ActionCallbackP1: public ActionCallbackBase<P> {
    std::function<R(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void setValueCb(P p) override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef R ResultType;

    ActionCallbackP1() = delete;
    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

template<typename R, typename P, typename... Args>
class ActionCallbackP1<Future<R>, P, Args...>: public ActionCallbackBase<P> {
    std::function<Future<R>(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void setValueCb(P p) override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef R ResultType;

    ActionCallbackP1() = delete;
    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

template<typename P, typename... Args>
class ActionCallbackP1<Future<void>, P, Args...>: public ActionCallbackBase<P> {
    std::function<Future<void>(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void setValueCb(P p) override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef void ResultType;

    ActionCallbackP1() = delete;
    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

template<typename P, typename... Args>
class ActionCallbackP1<void, P, Args...>: public ActionCallbackBase<P> {
    std::function<void(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void setValueCb(P p) override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef void ResultType;

    ActionCallbackP1() = delete;
    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

// Errors

template<typename... Args>
class ActionCallbackError: public ActionCallbackBase<void> {
    std::function<void(const FutureError&, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    ActionCallbackError() = delete;
    ActionCallbackError(std::shared_ptr<Loop> iLoop, std::function<void(const FutureError&, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

/** Value resolver callback class template. It call the function callback in case if the future resolved
 */
template<typename R, typename... Args>
class ActionCallbackErrorP1: public ActionCallbackBase<R> {
    std::function<R(const FutureError&, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>);

    void setValueCb(R r) override;
    void setErrorCb(const FutureError& iError) override;

public:
    ActionCallbackErrorP1() = delete;
    ActionCallbackErrorP1(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError&, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<Loop> getLoop() override;
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACK_H__ */
