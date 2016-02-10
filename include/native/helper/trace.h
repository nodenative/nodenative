#ifndef INCLUDE_NATIVE_HELPER_TRACE_H_
#define INCLUDE_NATIVE_HELPER_TRACE_H_

#include <iostream>
#include <stdexcept>
#include <sstream>

#define NNATIVE_INFO(log) std::cout << __FILE__ << ":" << __LINE__ << " (INFO): " << log << "\n";

#ifndef NNATIVE_NO_ASSERT

#define NNATIVE_ASSERT(condition) if(!(condition)) {\
    std::stringstream ss;\
    ss << __PRETTY_FUNCTION__ << ": Assertion \"" << #condition << "\" failed";\
    NNATIVE_INFO(ss.str());\
    throw std::runtime_error(ss.str());\
}

#define NNATIVE_ASSERT_MSG(condition, msg) if(!(condition)) {\
    std::stringstream ss;\
    ss << __PRETTY_FUNCTION__ << ": Assertion \"" << #condition << "\" failed. Message:" << msg;\
    NNATIVE_INFO(msg);\
    throw std::runtime_error(ss.str());\
}
#else /* NNATIVE_NO_ASSERT */

#define NNATIVE_ASSERT(condition)
#define NNATIVE_ASSERT_MSG(condition, msg)

#endif /* NNATIVE_NO_ASSERT */

#ifdef DEBUG

#include <string>
#include <cstdlib>

#define NNATIVE_DEBUG(log) std::cout << __FILE__ << ":" << __LINE__ << " (DBG): " << log << "\n";
#define NNATIVE_FCALL() native::helper::TraceFunction __currFunction(__FILE__, __LINE__, __PRETTY_FUNCTION__);
#define NNATIVE_MCALL() native::helper::TraceFunction __currFunction(__FILE__, __LINE__, __PRETTY_FUNCTION__);

namespace native {
namespace helper {

struct TraceFunction {
    const std::string _file;
    const unsigned int _line;
    const std::string _function;

    TraceFunction(const std::string& iFile, unsigned int iLine, const std::string& iFunction) :
            _file(iFile),
            _line(iLine),
            _function(iFunction) {
        std::cout << _file << ":" << _line << ":>> enter "<< _function << "\n";
    }

    ~TraceFunction() {
        std::cout << _file << ":" << _line << ":<< exit " << _function << "\n";
    }
};

} /* namespace native */
} /* namespace native */

#else

#define NNATIVE_DEBUG(log)
#define NNATIVE_FCALL()
#define NNATIVE_MCALL()

#endif /* DEBUG */

#endif /* INCLUDE_NATIVE_HELPER_TRACE_H_ */

