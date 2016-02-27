#ifndef __NATIVE_ERROR_HPP__
#define __NATIVE_ERROR_HPP__

#include "base/base_utils.hpp"

namespace native
{
    class exception
    {
    public:
        exception(const std::string& message)
            : message_(message)
        {}

        virtual ~exception() {}

        const std::string& message() const { return message_; }

    private:
        std::string message_;
    };

    class error
    {
    public:
        error() : uv_err_(0) {}
        error(int iErrCode) : uv_err_(iErrCode) {}
        ~error() = default;
        bool isError() { return (uv_err_ != 0); }
        operator bool() { return isError(); }
        virtual error& operator=(int iErrCode)
        {
            setError(iErrCode);
            return *this;
        }

        void setError(int iErrCode) { uv_err_ = iErrCode; }
        int code() const { return uv_err_; }

        virtual const char* name() const { return uv_err_ < 0 ? uv_err_name(uv_err_) : 0; }
        virtual const char* str() const { return uv_err_ < 0 ? uv_strerror(uv_err_) : 0; }

    private:
        int uv_err_;
    };
}


#endif /* __NATIVE_ERROR_HPP__ */
