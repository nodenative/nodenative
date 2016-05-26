#ifndef __NATIVE_LOOP_HPP__
#define __NATIVE_LOOP_HPP__

#include "./base/base_utils.hpp"
#include "helper/trace.hpp"

#include <thread>

namespace native {

class AsyncBase;
/**
 *  Class that represents the Loop instance.
 */
class Loop : public std::enable_shared_from_this<Loop> {
protected:
  class HandleDeleter {
  public:
    HandleDeleter() : _default(false) {}
    HandleDeleter(const bool iDefault) : _default(iDefault) {}
    void operator()(uv_loop_t *iLoop);

  private:
    bool _default;
  };

  /**
   *  Default constructor
   */
  Loop();

public:
  Loop(const Loop &) = delete;
  void operator=(const Loop &) = delete;

  /** Create Loop instance.
   * @param iDefault True if the default loop should be used
   * @return A loop instance
   */
  static std::shared_ptr<Loop> Create(const bool iDefault = false);

  /** Get current event loop from current thread if any.
   * @return The event loop instance from the current thread if exists.
   */
  static std::shared_ptr<Loop> GetInstance();

  static std::shared_ptr<Loop> GetInstanceOrCreateDefault();

  /** Get event loop instance from thread id.
   * @return The loop instance from the specified instance if exists.
   */
  static std::shared_ptr<Loop> GetInstance(const std::thread::id &iThreadId);

  /**
   *  Destructor
   */
  ~Loop();

  /**
   *  Returns internal handle for libuv functions.
   */
  uv_loop_t *get() { return _uv_loop.get(); }

  std::shared_ptr<Loop> getInstance() { return shared_from_this(); }

  /**
   * Returns true if the current thread is the same as event Loop thread;
   */
  bool isNotOnEventLoopThread() const;

  bool started() const { return _started; }

  const std::thread::id &getThreadId() const { return _threadId; }

  /**
   *  Runs the event Loop until the reference count drops to zero. Always returns zero.
   *  Internally, this function just calls uv_run() function.
   */
  bool run();

  /**
   *  Poll for new events once. Note that this function blocks if there are no pending events. Returns true when done
   * (no active handles
   *  or requests left), or non-zero if more events are expected (meaning you
   *  should run the event Loop again sometime in the future).
   *  Internally, this function just calls uv_run_once() function.
   */
  bool run_once();

  /**
   *  Poll for new events once but don't block if there are no pending events.
   *  Internally, this function just calls uv_run_once() function.
   */
  bool run_nowait();

  /**
   *  ...
   *  Internally, this function just calls uv_update_time() function.
   */
  void update_time();

  /**
   *  ...
   *  Internally, this function just calls uv_now() function.
   */
  uint64_t now();

private:
  std::unique_ptr<uv_loop_t, HandleDeleter> _uv_loop;
  bool _started;
  std::thread::id _threadId;
};

/**
 *  Starts the default Loop.
 */
bool run();

/**
 *  Polls for new events once for the default Loop.
 *  Note that this function blocks if there are no pending events. Returns true when done (no active handles
 *  or requests left), or non-zero if more events are expected (meaning you
 *  should run the event Loop again sometime in the future).
 */
bool run_once();

/**
 *  Polls for new events once but don't block if there are no pending events for the default Loop.
 */
bool run_nowait();

} /* namespace native */

#endif /* __NATIVE_LOOP_HPP__ */
