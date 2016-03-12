#ifndef __NATIVE_ASYNC_REQUESTPROMISE_HPP__
#define __NATIVE_ASYNC_REQUESTPROMISE_HPP__

#include "Promise.hpp"

namespace native {

template<typename R, typename Req>
struct RequestPromise {
    RequestPromise() = delete;
    RequestPromise(std::shared_ptr<native::Loop> iLoop,
                   std::shared_ptr<native::base::Stream> iInstance) :
            _promise(iLoop),
            _instance(iInstance) {
        _req.data = this;
    }

    Req _req;
    native::Promise<R> _promise;
    std::shared_ptr<native::base::Stream> _instance;
};

} /* namespace native */

#endif /* __NATIVE_ASYNC_REQUESTPROMISE_HPP__ */
