#ifndef __ASYNC_FUTUREERROR_HPP__
#define __ASYNC_FUTUREERROR_HPP__

#include "../Error.hpp"

namespace native {

class FutureError : public Exception {
public:
  FutureError(const std::string &iError) : Exception(iError) {}
  FutureError(const std::string &iError, const std::string &iFile, const int iLine, const std::string &iFunction)
      : Exception(iError, iFile, iLine, iFunction) {}
};

class PromiseAlreadySatisfied : public FutureError {
public:
  PromiseAlreadySatisfied() : FutureError("Promise already satisfied") {}
};

} /* namespace native */

#endif /* __ASYNC_FUTUREERROR_HPP__ */
