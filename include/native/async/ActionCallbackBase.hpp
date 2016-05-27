#ifndef __NATIVE_ASYNC_ACTIONCALLBACKBASE_HPP__
#define __NATIVE_ASYNC_ACTIONCALLBACKBASE_HPP__

/*-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
 * Propose :
 * -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#include "AsyncBase.hpp"
#include "FutureError.hpp"
#include "FutureSharedResolver.hpp"

namespace native {

/** Action callback base class template
 */
template <typename P> class ActionCallbackBase : public AsyncBase {
protected:
  ActionCallbackBase(std::shared_ptr<Loop> iLoop);
  void executeAsync() override;
  void closeAsync() override;

  std::unique_ptr<FutureSharedResolver<P>> _resolver;

public:
  virtual ~ActionCallbackBase() {}

  std::shared_ptr<ActionCallbackBase<P>> getInstance();

  void resolve(P p);
  void reject(const FutureError &);

  virtual void resolveCb(P) = 0;
  virtual void rejectCb(const FutureError &) = 0;
};

/** Acction callback base class template specialization for void type.
 */
template <> class ActionCallbackBase<void> : public AsyncBase {
protected:
  ActionCallbackBase(std::shared_ptr<Loop>);
  void executeAsync() override;
  void closeAsync() override;

  std::unique_ptr<FutureSharedResolver<void>> _resolver;

public:
  virtual ~ActionCallbackBase() {}

  std::shared_ptr<ActionCallbackBase<void>> getInstance();

  void resolve();
  void reject(const FutureError &iError);

  virtual void resolveCb() = 0;
  virtual void rejectCb(const FutureError &) = 0;
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_ACTIONCALLBACKBASE_HPP__ */
