#ifndef __NATIVE_WORKER_WORKERBASE_HPP__
#define __NATIVE_WORKER_WORKERBASE_HPP__

#include <uv.h>
#include "../Loop.hpp"
#include "../helper/trace.hpp"

#include <memory>

namespace native {

class WorkerBase: public std::enable_shared_from_this<WorkerBase> {
    std::shared_ptr<WorkerBase> _self;
    std::shared_ptr<Loop> _loop;
    std::shared_ptr<WorkerBase> _instance;

    // This method will be called from uv
    static void Worker(uv_work_t* iHandle);
    static void WorkerAfter(uv_work_t* iHandle, int iStatus);
protected:
    uv_work_t _uvWork;

    /** Public method called at event Loop
     * Extend this class for custom usage
     */
    virtual void executeWorker() = 0;
    virtual void executeWorkerAfter(int iStatus) = 0;
    virtual void closeWorker() {}
    void enqueue();
    WorkerBase() = delete;
    WorkerBase(std::shared_ptr<Loop> iLoop);
public:
    virtual ~WorkerBase();
    std::shared_ptr<Loop> getLoop() { return _loop; }
    std::shared_ptr<WorkerBase> getInstance() { return this->shared_from_this(); }
};

} /* namespace native */

#endif /* __NATIVE_WORKER_WORKERBASE_HPP__ */
