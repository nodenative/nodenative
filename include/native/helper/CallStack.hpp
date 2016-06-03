#ifndef __NATIVE_HELPER_CALLSTACK_HPP__
#define __NATIVE_HELPER_CALLSTACK_HPP__

#include <sstream>
#include <string>
#include <vector>

namespace native {
namespace helper {

/**
 * Call stack implementation class
 * The code is inspired from https://sourceforge.net/p/stacktrace/code/HEAD/tree/stacktrace/
 */
struct CallStack {
  struct Entry {
    Entry() : line(0) {}

    std::string toString() const {
      std::ostringstream os;
      os << file << " (" << line << "): " << function;
      return os.str();
    }

    std::string file;     ///< filename
    size_t line;          ///< line number
    std::string function; ///< name of function or method
  };

  CallStack(const size_t iDiscardNum = 0, const size_t iCaptureNum = 32);

  virtual ~CallStack() throw() {}

  /** Serializes the entire call-stack into a text string. */
  std::string toString() const {
    std::ostringstream os;
    for (const Entry &entry : stack) {
      os << entry.toString() << std::endl;
    }

    return os.str();
  }

  std::vector<Entry> stack;
};

} /* namespace helper */
} /* namespace native */

#endif /* __NATIVE_HELPER_CALLSTACK_HPP__ */
