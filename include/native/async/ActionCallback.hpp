#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_HPP__
#define __NATIVE_ASYNC_ACTIONCALLBACK_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.hpp"
#include "FutureError.hpp"
#include "AsyncBase.hpp"
#include "FutureSharedResolver.hpp"

namespace native {

template<class R>
class FutureShared;

template<typename R>
class Future;

/** Action callback base class template
 */
template<typename P>
class ActionCallbackBase : public std::enable_shared_from_this<ActionCallbackBase<P>>, public AsyncBase {
protected:
    ActionCallbackBase(std::shared_ptr<Loop> iLoop);
    void executeAsync() override;
    void closeAsync(std::unique_ptr<AsyncBase>) override;

    std::shared_ptr<ActionCallbackBase<P>> _instance;
    std::unique_ptr<FutureSharedResolver<P>> _resolver;

public:
    virtual ~ActionCallbackBase() {}

    std::shared_ptr<ActionCallbackBase<P>> getInstance();

    void resolve(P p);
    void reject(const FutureError&);

    virtual void resolveCb(P) = 0;
    virtual void rejectCb(const FutureError&) = 0;
};

/** Acction callback base class template specialization for void type.
 */
template<>
class ActionCallbackBase<void> : public std::enable_shared_from_this<ActionCallbackBase<void>>, public AsyncBase {
protected:
    ActionCallbackBase(std::shared_ptr<Loop>);
    void executeAsync() override;
    void closeAsync(std::unique_ptr<AsyncBase>) override;

    std::shared_ptr<ActionCallbackBase<void>> _instance;
    std::unique_ptr<FutureSharedResolver<void>> _resolver;

public:
    virtual ~ActionCallbackBase() {}

    std::shared_ptr<ActionCallbackBase<void>> getInstance();

    void resolve();
    void reject(const FutureError &iError);

    virtual void resolveCb() = 0;
    virtual void rejectCb(const FutureError&) = 0;
};

template<typename R, typename... Args>
class ActionCallback: public ActionCallbackBase<void> {
    std::function<R(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;
    void rejectCb(const FutureError& iError) override;
    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args);

public:
    typedef R ResultType;

    static std::shared_ptr<ActionCallback<R, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<R(Args...)> f, Args&&... args);

    ActionCallback() = delete;

    std::shared_ptr<FutureShared<R>> getFuture();
};

template<typename R, typename... Args>
class ActionCallback<Future<R>, Args...>: public ActionCallbackBase<void> {
    std::function<Future<R>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;
    void rejectCb(const FutureError& iError) override;
    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args);

public:
    typedef R ResultType;

    static std::shared_ptr<ActionCallback<Future<R>, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(Args...)> f, Args&&... args);

    ActionCallback() = delete;

    std::shared_ptr<FutureShared<R>> getFuture();
};

template<typename... Args>
class ActionCallback<Future<void>, Args...>: public ActionCallbackBase<void> {
    std::function<Future<void>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;
    void rejectCb(const FutureError& iError) override;

    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args);

public:
    typedef void ResultType;

    ActionCallback() = delete;
    static std::shared_ptr<ActionCallback<Future<void>, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
};

template<typename... Args>
class ActionCallback<void, Args...>: public ActionCallbackBase<void> {
    std::function<void(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void resolveCb() override;
    void rejectCb(const FutureError& iError) override;

    ActionCallback(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args);

public:
    typedef void ResultType;

    ActionCallback() = delete;
    static std::shared_ptr<ActionCallback<void, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<void(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
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

    void resolveCb(P p) override;
    void rejectCb(const FutureError& iError) override;

    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args&&... args);

public:
    typedef R ResultType;

    ActionCallbackP1() = delete;
    static std::shared_ptr<ActionCallbackP1<R, P, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<R(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
};

template<typename R, typename P, typename... Args>
class ActionCallbackP1<Future<R>, P, Args...>: public ActionCallbackBase<P> {
    std::function<Future<R>(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void resolveCb(P p) override;
    void rejectCb(const FutureError& iError) override;

    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args&&... args);

public:
    typedef R ResultType;

    ActionCallbackP1() = delete;
    static std::shared_ptr<ActionCallbackP1<Future<R>, P, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<Future<R>(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
};

template<typename P, typename... Args>
class ActionCallbackP1<Future<void>, P, Args...>: public ActionCallbackBase<P> {
    std::function<Future<void>(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void resolveCb(P p) override;
    void rejectCb(const FutureError& iError) override;

    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args&&... args);

public:
    typedef void ResultType;

    ActionCallbackP1() = delete;
    static std::shared_ptr<ActionCallbackP1<Future<void>, P, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<Future<void>(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
};

template<typename P, typename... Args>
class ActionCallbackP1<void, P, Args...>: public ActionCallbackBase<P> {
    std::function<void(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(P p, helper::TemplateSeqInd<Is...>);

    void resolveCb(P p) override;
    void rejectCb(const FutureError& iError) override;

    ActionCallbackP1(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args&&... args);

public:
    typedef void ResultType;

    ActionCallbackP1() = delete;
    static std::shared_ptr<ActionCallbackP1<void, P, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<void(P, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
};

// Errors

template<typename... Args>
class ActionCallbackError: public ActionCallbackBase<void> {
    std::function<void(const FutureError&, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(const FutureError& iError, helper::TemplateSeqInd<Is...>);

    void resolveCb() override;
    void rejectCb(const FutureError& iError) override;

    ActionCallbackError(std::shared_ptr<Loop> iLoop, std::function<void(const FutureError&, Args...)> f, Args&&... args);

public:
    ActionCallbackError() = delete;
    static std::shared_ptr<ActionCallbackError<Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<void(const FutureError&, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
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

    void resolveCb(R r) override;
    void rejectCb(const FutureError& iError) override;

    ActionCallbackErrorP1(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError&, Args...)> f, Args&&... args);

public:
    ActionCallbackErrorP1() = delete;
    static std::shared_ptr<ActionCallbackErrorP1<R, Args...>> Create(std::shared_ptr<Loop> iLoop, std::function<R(const FutureError&, Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACK_HPP__ */
