/* Liux/gcc implementation of the call_stack class. */
#ifdef __GNUC__

#include "native/helper/CallStack.hpp"

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

namespace native {
namespace helper {

CallStack::CallStack(const size_t iDiscardNum /* =0 */, const size_t iCaptureNum /* =32 */) {
  using namespace abi;

  // retrieve call-stack
  const size_t allocatedSize = iCaptureNum + iDiscardNum + 1;
  const size_t allocatedBytes = sizeof(void *) * allocatedSize;
  void **trace = (void **)malloc(allocatedBytes);
  int stackDepth = backtrace(trace, allocatedSize);

  for (int i = iDiscardNum + 1; i < stackDepth; ++i) {
    Dl_info dlinfo;
    if (!dladdr(trace[i], &dlinfo))
      break;

    const char *symname = dlinfo.dli_sname;

    int status;
    char *demangled = abi::__cxa_demangle(symname, NULL, 0, &status);
    if (status == 0 && demangled)
      symname = demangled;

    // printf("entry: %s, %s\n", dlinfo.dli_fname, symname);

    // store entry to stack
    if (dlinfo.dli_fname && symname) {
      Entry e;
      e.file = dlinfo.dli_fname;
      e.line = 0; // unsupported
      e.function = symname;
      stack.push_back(e);
    } else {
      break; // skip last entries below main
    }

    if (demangled)
      free(demangled);
  }

  free(trace);
}

} /* namespace helper */
} /* namespace native */

#endif // __GNUC__
