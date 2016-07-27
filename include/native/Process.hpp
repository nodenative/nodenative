#ifndef __NATIVE_PROCESS_HPP__
#define __NATIVE_PROCESS_HPP__

#include "Loop.hpp"
#include "base.hpp"

namespace native {

class Process : public native::base::Handle {
public:
  static std::shared_ptr<Process> Create();
  static std::shared_ptr<Process> Create(std::shared_ptr<native::Loop> iLoop);

  Process() = delete;
  ~Process();

  virtual void init();

  std::shared_ptr<Process> getInstanceProcess() {
    return std::static_pointer_cast<Process>(base::Handle::getInstanceHandle());
  }

  /**
   * Set on exit callback
   * @param cb process callback with the follo parameters: uint64_t - status, int - signal
   */
  std::shared_ptr<Process> onExit(std::function<void(uint64_t, int)> cb) {
    _exitCb = cb;
    return getInstanceProcess();
  }

  /**
   * Libuv can change the child process’ user/group id. This happens only when the appropriate bits are set in the flags
   * fields.
   * @note This is not supported on Windows, spawn() will fail and set the error to UV_ENOTSUP.
   * @param uid UID value
   */
  std::shared_ptr<Process> setUid(int uid);

  /**
   * Libuv can change the child process’ user/group id. This happens only when the appropriate bits are set in the flags
   * fields.
   * @note This is not supported on Windows, spawn() will fail and set the error to UV_ENOTSUP.
   * @param Gid GID value
   */
  std::shared_ptr<Process> setGid(int gid);

  Error spawn();

  /**
   * Sends the specified signal to the given process handle. Check the documentation on uv_signal_t — Signal handle for
   * signal support, specially on Windows.
   * @param  signal signal number
   * @return        True if method sucess, false otherwise
   */
  Error kill(int signal);

  /**
   * Sends the specified signal to the given PID. Check the documentation on uv_signal_t — Signal handle for signal
   * support, specially on Windows.
   * @param  pid    process ID
   * @param  signal signal number
   * @return        True if method sucess, false otherwise
   */
  static Error Kill(int pid, int signal);

protected:
  Process(std::shared_ptr<native::Loop> l);
  virtual void init(uv_handle_t *iHandlePtr) override;

private:
  uv_process_options_t *getProcessOptionsPtr();
  static void OnExit(uv_process_t *handle, int64_t exit_status, int term_signal);

  std::unique_ptr<uv_process_t> _uvProcessInstance;
  std::unique_ptr<uv_process_options_t> _uvProcessOptions;
  std::function<void(int64_t, int)> _exitCb;
};

} /* namespace native */

#endif /* __NATIVE_NET_TCP_HPP__ */
