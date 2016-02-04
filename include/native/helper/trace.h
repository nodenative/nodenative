#ifndef INCLUDE_NATIVE_HELPER_TRACE_H_
#define INCLUDE_NATIVE_HELPER_TRACE_H_

#ifdef DEBUG

#include <string>
#include <iostream>



#define NNATIVE_DEBUG(log) std::cout << log << "\n";
#define NNATIVE_FCALL() native::helper::TraceFunction __currFunction(__FUNCTION__);

namespace native {
namespace helper {

struct TraceFunction {
    std::string _name;
    TraceFunction(const std::string& iName) : _name(iName) {
        std::cout << ">> enter "<< _name << "\n";
    }

    ~TraceFunction() {
        std::cout << "<< exit " << _name << "\n";
    }
};

} /* namespace native */
} /* namespace native */

#else

#define NNATIVE_DEBUG(log)
#define NNATIVE_FCALL()

#endif /* DEBUG */

#endif /* INCLUDE_NATIVE_HELPER_TRACE_H_ */

