#ifndef __NATIVE_HANDLE_H__
#define __NATIVE_HANDLE_H__

#include "base_utils.hh"
#include "../callback.h"

namespace native
{
    namespace base
    {
        void _delete_handle(uv_handle_t* h);

        class Handle
        {
        public:
            template<typename T>
            Handle(T* x)
                : uv_handle_(reinterpret_cast<uv_handle_t*>(x))
            {
                //printf("Handle(): %x\n", this);
                assert(uv_handle_);

                uv_handle_->data = new callbacks(native::internal::uv_cid_max);
                assert(uv_handle_->data);
            }

            virtual ~Handle()
            {
                //printf("~Handle(): %x\n", this);
                uv_handle_ = nullptr;
            }

            Handle(const Handle& h)
                : uv_handle_(h.uv_handle_)
            {
                //printf("Handle(const Handle&): %x\n", this);
            }

        public:
            template<typename T=uv_handle_t>
            T* get() { return reinterpret_cast<T*>(uv_handle_); }

            template<typename T=uv_handle_t>
            const T* get() const { return reinterpret_cast<T*>(uv_handle_); }

            bool is_active() { return uv_is_active(get()) != 0; }

            void close(std::function<void()> callback)
            {
                callbacks::store(get()->data, native::internal::uv_cid_close, callback);
                uv_close(get(),
                    [](uv_handle_t* h) {
                        callbacks::invoke<decltype(callback)>(h->data, native::internal::uv_cid_close);
                        _delete_handle(h);
                    });
            }

            Handle& operator =(const Handle& h)
            {
                uv_handle_ = h.uv_handle_;
                return *this;
            }

        protected:
            uv_handle_t* uv_handle_;
        };
    }
}

#endif
