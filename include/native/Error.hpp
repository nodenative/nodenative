#ifndef __NATIVE_ERROR_HPP__
#define __NATIVE_ERROR_HPP__

#include "base/base_utils.hpp"

namespace native {
class Exception {
public:
  struct CallStack {
    CallStack() = delete;
    CallStack(const std::string &iFile, const int iLine, const std::string &iFunction)
        : _file(iFile), _line(iLine), _function(iFunction) {}
    std::string _file;
    int _line;
    std::string _function;
  };

  Exception(const std::string &message) : _message(message), _uvError(0) {}

  Exception(const std::string &message, const std::string &iFile, const int iLine, const std::string &iFunction)
      : _message(message), _uvError(0) {
    CallStack item(iFile, iLine, iFunction);
    _callStack.push_back(item);
  }

  Exception(const std::string &message,
            const std::string &iFile,
            const int iLine,
            const std::string &iFunction,
            const int iuvError)
      : _message(message), _uvError(iuvError) {
    CallStack item(iFile, iLine, iFunction);
    _callStack.push_back(item);
  }

  Exception(const Exception &iOther, const std::string &iFile, const int iLine, const std::string &iFunction) {
    CallStack item(iFile, iLine, iFunction);
    _callStack.push_back(item);
    for (const CallStack &i : iOther._callStack) {
      _callStack.push_back(i);
    }
    _message = iOther._message;
    _uvError = iOther._uvError;
  }

  virtual ~Exception() {}

  const std::string &message() const { return _message; }

private:
  std::string _message;
  std::list<CallStack> _callStack;
  int _uvError;
};

class Error {
public:
  Error() : _uv_err(0) {}
  Error(int iErrCode) : _uv_err(iErrCode) {}
  Error(const std::string &str) : _uv_err(-1), _str(str) {}
  ~Error() = default;
  bool isError() { return (_uv_err < 0); }
  bool isOK() { return !isError(); }
  operator bool() { return isError(); }
  virtual Error &operator=(int iErrCode) {
    setError(iErrCode);
    return *this;
  }

  void setError(int iErrCode) { _uv_err = iErrCode; }
  int code() const { return _uv_err; }

  virtual const char *name() const {
    if (_str.empty() && _uv_err < 0) {
      return uv_err_name(_uv_err);
    }
    return 0;
  }
  virtual const char *str() const {
    if (!_str.empty()) {
      return _str.c_str();
    } else if (_uv_err < 0) {
      return uv_strerror(_uv_err);
    }
    return 0;
  }

private:
  int _uv_err;
  std::string _str;
};
}

#endif /* __NATIVE_ERROR_HPP__ */
