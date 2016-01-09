#ifndef INCLUDE_NATIVE_BASE_H_
#define INCLUDE_NATIVE_BASE_H_

#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <map>
#include <algorithm>
#include <list>
#include <set>
#include <tuple>

#include <uv.h>

namespace native {

namespace internal {
enum uv_callback_id {
    uv_cid_close = 0,
    uv_cid_listen,
    uv_cid_read_start,
    uv_cid_write,
    uv_cid_shutdown,
    uv_cid_connect,
    uv_cid_connect6,
    uv_cid_max
};
}  // namespace internal

// TODO(ionlupascu): to finish this class
class BaseObject {
};


}  // namespace native

#endif  // INCLUDE_NATIVE_BASE_H_
