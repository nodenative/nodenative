#ifndef __NATIVE_HANDLE_HANDLE_HPP__
#define __NATIVE_HANDLE_HANDLE_HPP__

#include "base_utils.hpp"
#include "../Loop.hpp"
#include "../async.hpp"
#include "../async.ipp"

namespace native {
namespace base {

class Handle : public std::enable_shared_from_this<Handle> {
public:
    Handle() = delete;
    Handle& operator =(const Handle& h) = delete;
    Handle(const Handle& h) = delete;

    virtual ~Handle();

    template<typename T=uv_handle_t>
    T* get() {
        NNATIVE_ASSERT(_uvHandlePtr != nullptr);
        return reinterpret_cast<T*>(_uvHandlePtr);
    }

    template<typename T=uv_handle_t>
    const T* get() const {
        NNATIVE_ASSERT(_uvHandlePtr != nullptr);
        return reinterpret_cast<T*>(_uvHandlePtr);
    }

    std::shared_ptr<Handle> getInstanceHandle();

    bool isActive();
    bool isClosing();

    void ref();
    void unref();
    Future<void> close();


    virtual void init(uv_handle_t* iHandlePtr);

protected:
    Handle(std::shared_ptr<native::Loop> iLoop);

    void keepInstanceHandle();
    void releaseInstanceHandle();

    uv_handle_t* _uvHandlePtr;
    std::shared_ptr<Loop> _loop;
    std::shared_ptr<Handle> _instanceHandle;
    unsigned int _instanceHandleCount;

private:
    Promise<void> _closingPromise;
};

} /* namespace base */
} /* namespace native */

#endif /* __NATIVE_HANDLE_HANDLE_HPP__ */
