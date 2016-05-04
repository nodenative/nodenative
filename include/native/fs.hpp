#ifndef __NATIVE_FS_H__
#define __NATIVE_FS_H__

#include "base.hpp"
#include <functional>
#include <algorithm>
#include <fcntl.h>
#include "Error.hpp"

namespace native
{
// TODO: implement functions that accept Loop pointer as extra argument.
namespace fs {

typedef uv_file file_handle;

extern const int read_only;
extern const int write_only;
extern const int read_write;
extern const int append;
extern const int create;
extern const int excl;
extern const int truncate;
extern const int no_follow;
extern const int directory;
#ifdef O_NOATIME
extern const int no_access_time;
#endif
#ifdef O_LARGEFILE
extern const int large_large;
#endif

int stringToFlags(const std::string &str);

Future<file_handle> open(const std::string& path, int flags, int mode);
file_handle openSync(const std::string& path, int flags, int mode);

Future<std::shared_ptr<std::string>> read(file_handle fd, size_t len, off_t offset);

Future<int> write(file_handle fd, const char* buf, size_t len, off_t offset);

/*
bool read_to_end(file_handle fd, std::function<void(const std::string& str, Error e)> callback);

bool close(file_handle fd, std::function<void(Error e)> callback);

bool unlink(const std::string& path, std::function<void(Error e)> callback);

bool mkdir(const std::string& path, int mode, std::function<void(Error e)> callback);

bool rmdir(const std::string& path, std::function<void(Error e)> callback);

bool rename(const std::string& path, const std::string& new_path, std::function<void(Error e)> callback);

bool chmod(const std::string& path, int mode, std::function<void(Error e)> callback);

bool chown(const std::string& path, int uid, int gid, std::function<void(Error e)> callback);

#if 0
bool readdir(const std::string& path, int flags, std::function<void(Error e)> callback);

bool stat(const std::string& path, std::function<void(Error e)> callback);

bool fstat(const std::string& path, std::function<void(Error e)> callback);
#endif
*/
} /* namespace fs */

class file
{
public:
static bool read(const std::string& path, std::function<void(const std::string& str, Error e)> callback);

static bool write(const std::string& path, const std::string& str, std::function<void(int nwritten, Error e)> callback);
};

} /* namespace native */

#endif /* __NATIVE_FS_H__ */
