#ifndef __NATIVE_ASYNC_ASYNCCALLBACK_H__
#define __NATIVE_ASYNC_ASYNCCALLBACK_H__

#include "AsyncBase.h"

namespace native {

template<class R, typename... Args>
class AsyncCallback : public AsyncBase {
    std::function<R(Args...)> _f;
    std::tuple<Args...> _args;
    Promise<R> _promise;

    template<std::size_t... Is>
    void executeCb(std::tuple<Args...>& iArgs, helper::TemplateSeqInd<Is...>) {
        try {
            _promise.setValue(_f(std::forward<Args>(std::get<Is>(iArgs))...));
        } catch (const FutureError &e) {
            _promise.setError(e);
        }
    }

protected:
    AsyncCallback(loop &iLoop, std::function<R(Args...)> f, Args&&... args) : AsyncBase(iLoop), _f(f), _args(args...) {}

    virtual void executeAsync(std::shared_ptr<AsyncBase>) {
        executeCb(_args, helper::TemplateSeqIndGen<sizeof...(Args)>{});
    }

public:

    static Future<R> Create(loop &iLoop, std::function<R(Args...)> f, Args&&... args) {
        NNATIVE_FCALL();
        using ThisType = AsyncCallback<R, Args...>;
        std::unique_ptr<ThisType> thisInst(new ThisType(iLoop, f, std::forward<Args>(args)...));
        thisInst->enqueue();
        return thisInst.release()->getFuture();
    }

    Future<R> getFuture() {
        NNATIVE_FCALL();
        return _promise.getFuture();
    }
};

template<typename... Args>
class AsyncCallback<void, Args...> : public AsyncBase {
    std::function<void(Args...)> _f;
    std::tuple<Args...> _args;
    Promise<void> _promise;

    template<std::size_t... Is>
    void executeCb(std::tuple<Args...>& iArgs, helper::TemplateSeqInd<Is...>) {
        try {
            _f(std::get<Is>(iArgs)...);
            _promise.setValue();
        } catch (const FutureError &e) {
            _promise.setError(e);
        }
    }

protected:
    AsyncCallback(loop &iLoop, std::function<void(Args...)> f, Args&&... args) : AsyncBase(iLoop), _f(f), _args(args...) {
        NNATIVE_FCALL();
    }

    virtual void executeAsync(std::shared_ptr<AsyncBase>) {
        NNATIVE_FCALL();
        executeCb(_args, helper::TemplateSeqIndGen<sizeof...(Args)>{});
    }

public:
    ~AsyncCallback() {
        NNATIVE_FCALL();
    }

    static Future<void> Create(loop &iLoop, std::function<void(Args...)> f, Args&&... args) {
        NNATIVE_FCALL();
        using ThisType = AsyncCallback<void, Args...>;
        std::unique_ptr<ThisType> thisInst(new ThisType(iLoop, f, std::forward<Args>(args)...));
        thisInst->enqueue();
        return thisInst.release()->getFuture();
    }

    Future<void> getFuture() {
        NNATIVE_FCALL();
        return _promise.getFuture();
    }
};

} /* namespace native */


#endif /* __NATIVE_ASYNC_ASYNCCALLBACK_H__ */
