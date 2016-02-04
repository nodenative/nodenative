#include "native/fs.h"

namespace native {

const int read_only = O_RDONLY;
const int write_only = O_WRONLY;
const int read_write = O_RDWR;
const int append = O_APPEND;
const int create = O_CREAT;
const int excl = O_EXCL;
const int truncate = O_TRUNC;
const int no_follow = O_NOFOLLOW;
const int directory = O_DIRECTORY;
#ifdef O_NOATIME
const int no_access_time = O_NOATIME;
#endif
#ifdef O_LARGEFILE
const int large_large = O_LARGEFILE;
#endif

namespace fs {
namespace internal {

void delete_req(uv_fs_t* req) {
    delete reinterpret_cast<callbacks*>(req->data);
    uv_fs_req_cleanup(req);
    delete req;
}

} // namespace internal
} // namespace fs
} // namespace native
