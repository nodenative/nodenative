#ifndef __NATIVE_ASYNC_REQUESTPROMISE_HPP__
#define __NATIVE_ASYNC_REQUESTPROMISE_HPP__

#include "Promise.hpp"
#include <memory>

namespace native {

template <typename R, typename Req> struct RequestPromise {
  RequestPromise() = delete;
  RequestPromise(std::shared_ptr<native::Loop> iLoop) : _promise(iLoop) { _req.data = this; }

  RequestPromise(std::shared_ptr<native::Loop> iLoop, std::function<void(Req *)> iDestructor)
      : _promise(iLoop), _destructor(iDestructor) {
    _req.data = this;
  }

  ~RequestPromise() {
    if (_destructor) {
      _destructor(&_req);
    }
  }

  static std::unique_ptr<RequestPromise<R, Req>> Create(std::shared_ptr<native::Loop> iLoop) {
    return std::make_unique<RequestPromise<R, Req>>(iLoop);
  }

  static std::unique_ptr<RequestPromise<R, Req>> Create(std::shared_ptr<native::Loop> iLoop,
                                                        std::function<void(Req *)> iDestructor) {
    return std::make_unique<RequestPromise<R, Req>>(iLoop, iDestructor);
  }

  Req _req;
  native::Promise<R> _promise;
  std::function<void(Req *)> _destructor;
};

template <typename R, typename Req, typename Instance> struct RequestPromiseInstance {
  RequestPromiseInstance() = delete;
  RequestPromiseInstance(std::shared_ptr<native::Loop> iLoop, std::shared_ptr<Instance> iInstance)
      : _promise(iLoop), _instance(iInstance) {
    _req.data = this;
  }

  static std::unique_ptr<RequestPromiseInstance<R, Req, Instance>> Create(std::shared_ptr<native::Loop> iLoop,
                                                                          std::shared_ptr<Instance> iInstance) {
    return std::make_unique<RequestPromiseInstance<R, Req, Instance>>(iLoop, iInstance);
  }

  Req _req;
  native::Promise<R> _promise;
  std::shared_ptr<Instance> _instance;
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_REQUESTPROMISE_HPP__ */
