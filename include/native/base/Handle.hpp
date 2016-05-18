#ifndef __NATIVE_HANDLE_HANDLE_HPP__
#define __NATIVE_HANDLE_HANDLE_HPP__

#include "../Loop.hpp"
#include "../async.hpp"
#include "../async.ipp"
#include "base_utils.hpp"

namespace native {

class Error;

namespace base {

class Handle : public std::enable_shared_from_this<Handle> {
public:
  Handle() = delete;
  Handle &operator=(const Handle &h) = delete;
  Handle(const Handle &h) = delete;

  virtual ~Handle();

  template <typename T = uv_handle_t> T *get() {
    NNATIVE_ASSERT(_uvHandlePtr != nullptr);
    return reinterpret_cast<T *>(_uvHandlePtr);
  }

  template <typename T = uv_handle_t> const T *get() const {
    NNATIVE_ASSERT(_uvHandlePtr != nullptr);
    return reinterpret_cast<T *>(_uvHandlePtr);
  }

  std::shared_ptr<Handle> getInstanceHandle();
  std::shared_ptr<Loop> getLoop() { return _loop; }

  bool isActive();

  /**
   * Reference the given handle. References are idempotent, that is, if a handle
   * is already referenced calling this function again will have no effect.
   */
  void ref();

  /**
   * Un-reference the given handle. References are idempotent, that is, if a handle
   * is not referenced calling this function again will have no effect.
   */
  void unref();

  /**
   * Returns `true` if the handle referenced, `false` otherwise.
   */
  bool hasRef();

  /**
   * Request handle to be closed. A future object will be returned.
   * This MUST be called on each handle before memory is released.
   *
   * In-progress requests, like uv_connect_t or uv_write_t, are cancelled and
   * have their callbacks called asynchronously with status=UV_ECANCELED.
   */
  Future<std::shared_ptr<Handle>> close();

  /// Returns `true` if the handle is closing or closed, `false` otherwise.
  bool isClosing();

  virtual void init(uv_handle_t *iHandlePtr);

protected:
  Handle(std::shared_ptr<native::Loop> iLoop);

  void keepInstanceHandle();
  void releaseInstanceHandle();

  uv_handle_t *_uvHandlePtr;
  std::shared_ptr<Loop> _loop;
  std::shared_ptr<Handle> _instanceHandle;
  unsigned int _instanceHandleCount;

private:
  Promise<std::shared_ptr<Handle>> _closingPromise;
};

} /* namespace base */
} /* namespace native */

#endif /* __NATIVE_HANDLE_HANDLE_HPP__ */
