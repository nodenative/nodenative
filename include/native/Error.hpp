#ifndef __NATIVE_ERROR_HPP__
#define __NATIVE_ERROR_HPP__

#include "base/base_utils.hpp"

namespace native
{
    class Exception
    {
    public:
        struct CallStack {
            CallStack() = delete;
            CallStack(const std::string& iFile,
                      const int iLine,
                      const std::string& iFunction):
                _file(iFile),
                _line(iLine),
                _function(iFunction)
            {
            }
            std::string _file;
            int _line;
            std::string _function;
        };

        Exception(const std::string& message)
            : _message(message),
            _uvError(0)
        {}

        Exception(const std::string& message,
                  const std::string& iFile,
                  const int iLine,
                  const std::string& iFunction)
            : _message(message),
            _uvError(0)
        {
            CallStack item(iFile, iLine, iFunction);
            _callStack.push_back(item);
        }

        Exception(const std::string& message,
                  const std::string& iFile,
                  const int iLine,
                  const std::string& iFunction,
                  const int iuvError)
            : _message(message),
            _uvError(iuvError)
        {
            CallStack item(iFile, iLine, iFunction);
            _callStack.push_back(item);
        }

        Exception(const Exception& iOther,
                  const std::string& iFile,
                  const int iLine,
                  const std::string& iFunction)
        {
            CallStack item(iFile, iLine, iFunction);
            _callStack.push_back(item);
            for(const CallStack& i : iOther._callStack) {
                _callStack.push_back(i);
            }
            _message = iOther._message;
            _uvError = iOther._uvError;
        }

        virtual ~Exception() {}

        const std::string& message() const { return _message; }

    private:
        std::string _message;
        std::list<CallStack> _callStack;
        int _uvError;
    };

    class Error
    {
    public:
        Error() : _uv_err(0) {}
        Error(int iErrCode) : _uv_err(iErrCode) {}
        ~Error() = default;
        bool isError() { return (_uv_err != 0); }
        operator bool() { return isError(); }
        virtual Error& operator=(int iErrCode)
        {
            setError(iErrCode);
            return *this;
        }

        void setError(int iErrCode) { _uv_err = iErrCode; }
        int code() const { return _uv_err; }

        virtual const char* name() const { return _uv_err < 0 ? uv_err_name(_uv_err) : 0; }
        virtual const char* str() const { return _uv_err < 0 ? uv_strerror(_uv_err) : 0; }

    private:
        int _uv_err;
    };
}


#endif /* __NATIVE_ERROR_HPP__ */
