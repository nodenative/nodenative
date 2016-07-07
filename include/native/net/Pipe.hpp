#ifndef __NATIVE_NET_PIPE_HPP__
#define __NATIVE_NET_PIPE_HPP__

#include "../Loop.hpp"
#include "../base.hpp"
#include "net_utils.hpp"

namespace native {
namespace net {

class Pipe : public native::base::Stream {
public:
  static std::shared_ptr<Pipe> Create(const bool ipc);
  static std::shared_ptr<Pipe> Create(std::shared_ptr<native::Loop> iLoop, const bool ipc);

  Pipe() = delete;
  ~Pipe();

  /**
   * Bind the pipe to a file path (Unix) or a name (Windows).
   * @note Paths on Unix get truncated to sizeof(sockaddr_un.sun_path) bytes, typically between 92 and 108 bytes.
   */
  bool bind(const std::string &name, Error &oError);
  bool bind(const std::string &name);

  /**
   * Connect to the Unix domain socket or the named pipe.
   * @note: Paths on Unix get truncated to sizeof(sockaddr_un.sun_path) bytes, typically between 92 and 108 bytes.
   */
  Future<void> connect(const std::string &name);

  virtual void init();

  std::shared_ptr<Pipe> getInstancePipe() {
    return std::static_pointer_cast<Pipe>(native::base::Stream::getInstanceStream());
  }

  /**
   * Get the name of the Unix domain socket or the named pipe.
   * @param  name socket name to be returned
   * @return      TRUE if no errors, FALSE otherwise
   */
  bool getsockname(std::string &name);

  /**
   * Get the name of the Unix domain socket or the named pipe to which the handle is connected.
   * @param  name peer name to be returned
   * @return      TRUE if no errors, FALSE otherwise
   */
  bool getpeername(std::string &name);

  /**
   * Pending handles count
   * @return number pending haandles
   */
  int pendingCount();

  /**
   * Used to receive handles over IPC pipes.
   * First - call pendingCount(),
   * if it’s > 0 then initialize a handle of the given type, returned by pendingType() and call accept(pipe, handle).
   * @return pending handle type
   */
  int pendingType();

  /**
   * Set the number of pending pipe instance handles when the pipe server is waiting for connections.
   *
   * @note This setting applies to Windows only.
   * @param  instances number of instances
   * @return           True if updated, False otherwise.
   */
  bool setPendingInstances(int instances);

protected:
  Pipe(std::shared_ptr<native::Loop> l, const bool ipc);
  virtual void init(uv_handle_t *iHandlePtr) override;

private:
  static void Connect(uv_connect_t *req, int status);
  std::unique_ptr<uv_pipe_t> _uvPipeInstance;
  bool _ipc;
  bool _connecting;
  bool _connected;
};

} /* namespace net */
} /* namespace native */

#endif /* __NATIVE_NET_TCP_HPP__ */
