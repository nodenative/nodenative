#ifndef __NATIVE_ASYNC_ASYNCBASE_HPP__
#define __NATIVE_ASYNC_ASYNCBASE_HPP__

#include "../Loop.hpp"
#include "../helper/trace.hpp"
#include <uv.h>

#include <memory>

namespace native {

class AsyncBase : public std::enable_shared_from_this<AsyncBase> {
  std::shared_ptr<AsyncBase> _self;
  std::shared_ptr<Loop> _loop;
  std::shared_ptr<AsyncBase> _instance;

  // This method will be called from uv
  static void Async(uv_async_t *);
  static void AsyncClosed(uv_handle_t *);

protected:
  uv_async_t _uv_async;

  /** Public method called at event Loop
   * Extend this class for custom usage
   */
  virtual void executeAsync() = 0;
  virtual void closeAsync() {}
  void enqueue();
  AsyncBase() = delete;
  AsyncBase(std::shared_ptr<Loop> iLoop);

public:
  virtual ~AsyncBase();
  std::shared_ptr<Loop> getLoop() { return _loop; }
  std::shared_ptr<AsyncBase> getInstance() { return this->shared_from_this(); }
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ASYNCBASE_HPP__ */
