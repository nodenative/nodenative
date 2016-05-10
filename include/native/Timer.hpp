#ifndef __NATIVE_TIMER_HPP__
#define __NATIVE_TIMER_HPP__

#include "base/Handle.hpp"

namespace native {

class Timer : public base::Handle {
protected:
  Timer(std::shared_ptr<Loop> iLoop, std::function<void()> iCallback);

public:
  Timer() = delete;
  virtual ~Timer();

  /**
   * Create Timer instance. The `init` method is called automatically.
   * No need to keep a copy of instance, the timer is self holding till it will be closed.
   *
   * @param iLoop - loop instance
   */
  static std::shared_ptr<Timer> Create(std::shared_ptr<Loop> iLoop, std::function<void()> iCallback = {});

  std::shared_ptr<Timer> getInstanceTimer();

  /**
   * Initiate instance handler. This method is called automatically at Create.
   * It may be useful to re-initiate a closed Timer.
   */
  void init();
  void init(uv_handle_t *iHandle);

  void onTimeout(std::function<void()> iCallback) { _callback = iCallback; }

  /**
   * Start the timer, iTimeout and iRepeat are in milliseconds.
   * @param iTimeout if it is zero, the callback fires on the next event loop iteration.
   * @param iRepeat if it is non-zero, the callback fires first after `iTimeout` milliseconds and then repeatedly after
   * `iRepeat` milliseconds.
   */
  Error start(const uint64_t iTimeout, const uint64_t iRepeat);

  /**
   * Stop the timer, the callback will not be called anymore.
   */
  Error stop();

  /// Get the timer repeat value.
  uint64_t getRepeat();

  /**
   * Set the repeat interval value in milliseconds.
   * The timer will be scheduled to run on the given interval,
   * regardless of the callback execution duration, and will follow normal timer semantics in the case of a time-slice
   * overrun.
   *
   * For example, if a 50ms repeating timer first runs for 17ms, it will be
   * scheduled to run again 33ms later. If other tasks consume more than the
   * 33ms following the first timer callback, then the callback will run as soon as possible.
   *
   * @note If the repeat value is set from a timer callback it does not immediately take effect.
   * If the timer was non-repeating before, it will have been stopped. If it was repeating,
   * then the old repeat value will have been used to schedule the next timeout.
   */
  void setRepeat(const uint64_t iRepeat);

  /**
   * Stop the timer, and if it is repeating restart it using the repeat value as the timeout.
   * If the timer has never been started before it returns UV_EINVAL.
   */
  Error again();

  Future<std::shared_ptr<Timer>> close();
  void onClose(const std::function<void(std::shared_ptr<Timer>)> iOnClose);

protected:
  static void OnTimeout(uv_timer_t *iHandle);

private:
  uv_timer_t _uvTimer;
  std::function<void()> _callback;
  std::function<void(std::shared_ptr<Timer>)> _onClose;
};

} /* namespace native */

#endif /* __NATIVE_TIMER_HPP__ */
