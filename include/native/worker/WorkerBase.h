#ifndef __NATIVE_WORKER_WORKERBASE_H__
#define __NATIVE_WORKER_WORKERBASE_H__

#include <uv.h>
#include "../loop.h"
#include "../helper/trace.h"

#include <memory>

namespace native {

class WorkerBase {
    std::shared_ptr<WorkerBase> _self;
    std::shared_ptr<uv_loop_t> _loop;

    // This method will be called from uv
    static void Worker(uv_work_t* iHandle);
    static void WorkerAfter(uv_work_t* iHandle, int iStatus);
protected:
    uv_work_t _uvWork;

    /** Public method called at event loop
     * Extend this class for custom usage
     */
    virtual void executeWorker() = 0;
    virtual void executeWorkerAfter(int iStatus) = 0;
    virtual void closeWorker(std::unique_ptr<WorkerBase>) {}
    void enqueue();
    WorkerBase() = delete;
    WorkerBase(loop &iLoop);
    WorkerBase(std::shared_ptr<uv_loop_t> iLoop);
public:
    template<class Child, typename... Args>
    static Child* Create(loop& iLoop, Args&&... args);
    virtual ~WorkerBase();
};

} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERBASE_H__ */
