#ifndef __NATIVE_WORKER_WORKERCALLBACK_H__
#define __NATIVE_WORKER_WORKERCALLBACK_H__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "../helper/TemplateSeqInd.h"
#include "../async/FutureError.h"
#include "WorkerBase.h"

namespace native {

template<class R>
class FutureShared;

template<typename R>
class Future;

/** Acction callback base class template specialization for void type.
 */
class WorkerCallbackBase {
protected:
    std::weak_ptr<WorkerCallbackBase> _instance;

public:
    virtual ~WorkerCallbackBase() {}

    static void SetValue(std::shared_ptr<WorkerCallbackBase> iInstance);

    virtual void setValueCb() = 0;
    virtual void setErrorCb(const FutureError&) = 0;
    virtual std::shared_ptr<uv_loop_t> getLoop() = 0;
};

class WorkerCallbackBaseDetached : public WorkerBase {
private:
    std::shared_ptr<WorkerCallbackBase> _instance;

public:
    WorkerCallbackBaseDetached() = delete;
    WorkerCallbackBaseDetached(std::shared_ptr<WorkerCallbackBase> iInstance);
    ~WorkerCallbackBaseDetached() {
        NNATIVE_FCALL();
    }

    void executeWorker() override;
    void executeWorkerAfter(int iStatus) override;

    static void Enqueue(std::shared_ptr<WorkerCallbackBase> iInstance);
};

template<typename R, typename... Args>
class WorkerCallback: public WorkerCallbackBase {
    std::function<R(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef R ResultType;

    WorkerCallback() = delete;
    WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<R(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<uv_loop_t> getLoop() override;
};

template<typename R, typename... Args>
class WorkerCallback<Future<R>, Args...>: public WorkerCallbackBase {
    std::function<Future<R>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<R>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef R ResultType;

    WorkerCallback() = delete;
    WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<Future<R>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<R>> getFuture();
    std::shared_ptr<uv_loop_t> getLoop() override;
};

template<typename... Args>
class WorkerCallback<Future<void>, Args...>: public WorkerCallbackBase {
    std::function<Future<void>(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef void ResultType;

    WorkerCallback() = delete;
    WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<Future<void>(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<uv_loop_t> getLoop() override;
};

template<typename... Args>
class WorkerCallback<void, Args...>: public WorkerCallbackBase {
    std::function<void(Args...)> _f;
    std::tuple<Args...> _args;
    std::shared_ptr<FutureShared<void>> _future;

    template<std::size_t... Is>
    void callFn(helper::TemplateSeqInd<Is...>);

    void setValueCb() override;
    void setErrorCb(const FutureError& iError) override;

public:
    typedef void ResultType;

    WorkerCallback() = delete;
    WorkerCallback(std::shared_ptr<uv_loop_t> iLoop, std::function<void(Args...)> f, Args&&... args);

    std::shared_ptr<FutureShared<void>> getFuture();
    std::shared_ptr<uv_loop_t> getLoop() override;
};

} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERCALLBACK_H__ */
