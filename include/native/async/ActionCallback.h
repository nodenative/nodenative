#ifndef __NATIVE_ASYNC_ACTIONCALLBACK_H__
#define __NATIVE_ASYNC_ACTIONCALLBACK_H__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * Created By : ionlupascu
 * Creation Date : 26-01-2016
 * Last Modified : Thu 04 Feb 2016 06:45:59 GMT
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.h"
#include "../error.h"

namespace native {

template<class R>
class FutureShared;

/** Action callback base class template
 */
template<typename P>
class ActionCallbackBase {
public:
    virtual ~ActionCallbackBase() {}
    virtual void setValue(P&&) = 0;
    virtual void setException(const exception&) = 0;
};

/** Acction callback base class template specialization for void type.
 */
template<>
class ActionCallbackBase<void> {
public:
    virtual ~ActionCallbackBase() {}
    virtual void setValue() = 0;
    virtual void setException(const exception&) = 0;
};

template<typename R, typename... Args>
class ActionCallback: public ActionCallbackBase<void> {
    std::function<R(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

public:
    ActionCallback(std::function<R(Args...)> f, Args&&... args) : _f(f), _args(args...) {}

    void setValue() override {
        callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
    }

    void setException(const exception& iError) override;

    std::shared_ptr<FutureShared<R>> getFuture();
};

template<typename... Args>
class ActionCallback<void, Args...>: public ActionCallbackBase<void> {
    std::function<void(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

public:
    ActionCallback(std::function<void(Args...)> f, Args&&... args) : _f(f), _args(args...) {}

    void setValue() override {
        callFn(helper::TemplateSeqIndGen<sizeof...(Args)>());
    }

    void setException(const exception& iError) override;

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
    void callFn(P&& p, helper::TemplateSeqInd<Is...>);
public:
    ActionCallbackP1(std::function<R(P, Args...)> f, Args&&... args) : _f(f), _args(args...) {}

    void setValue(P&& p) override {
        callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
    }

    void setException(const exception& iError) override;
    std::shared_ptr<FutureShared<R>> getFuture();
};

template<typename P, typename... Args>
class ActionCallbackP1<void, P, Args...>: public ActionCallbackBase<P> {
    std::function<void(P, Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(P&& p, helper::TemplateSeqInd<Is...>);

public:
    ActionCallbackP1(std::function<void(P, Args...)> f, Args&&... args) : _f(f), _args(args...) {}

    void setValue(P&& p) override {
        callFn(std::forward<P>(p), helper::TemplateSeqIndGen<sizeof...(Args)>());
    }

    void setException(const exception& iError) override;
    std::shared_ptr<FutureShared<void>> getFuture();
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACK_H__ */
