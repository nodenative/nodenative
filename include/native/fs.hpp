#ifndef __NATIVE_FS_H__
#define __NATIVE_FS_H__

#include "Error.hpp"
#include "base.hpp"
#include <algorithm>
#include <fcntl.h>
#include <functional>

namespace native {
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

Future<file_handle> open(const std::string &path, const int flags = O_RDONLY, const int mode = 0666);
Future<file_handle> open(const std::string &path, const std::string &flags, const int mode = 0666);
file_handle openSync(const std::string &path, const int flags = O_RDONLY, const int mode = 0666);
file_handle openSync(const std::string &path, const std::string &flags, const int mode = 0666);

Future<std::shared_ptr<std::string>> read(const file_handle fd, const size_t len, const off_t offset = 0);
std::shared_ptr<std::string> readSync(const file_handle fd, const size_t len, const off_t offset = 0);

Future<int> write(const file_handle fd, const char *buf, const size_t len, const off_t offset = 0);
Future<int> write(const file_handle fd, const std::string &buf, const off_t offset = 0);
int writeSync(const file_handle fd, const char *buf, const size_t len, const off_t offset = 0);
int writeSync(const file_handle fd, const std::string &buf, const off_t offset = 0);

/*
bool read_to_end(file_handle fd, std::function<void(const std::string& str,
Error e)> callback);
*/

Future<void> access(const file_handle fd, const int mode);
void accessSync(const file_handle fd, const int mode);

Future<void> close(const file_handle fd);
void closeSync(const file_handle fd);

Future<void> unlink(const std::string &path);
void unlinkSync(const std::string &path);

Future<void> mkdir(const std::string &path, const int mode);
void mkdirSync(const std::string &path, const int mode);

Future<void> rmdir(const std::string &path);
void rmdirSync(const std::string &path);

Future<void> rename(const std::string &path, const std::string &new_path);
void renameSync(const std::string &path, const std::string &new_path);

Future<void> chmod(const std::string &path, const int mode);
void chmodSync(const std::string &path, const int mode);

Future<void> fchmod(const file_handle fd, const int mode);
void fchmodSync(const file_handle fd, const int mode);

Future<void> chown(const std::string &path, const int uid, const int gid);
void chownSync(const std::string &path, const int uid, const int gid);

Future<void> scandir(const std::string &path, const int flags);
void scandirSync(const std::string &path, const int flags);

Future<void> scandirNext(const std::string &path, const int flags);
void scandirNextSync(const std::string &path, const int flags);

#if 0
bool readdir(const std::string& path, int flags, std::function<void(Error e)> callback);

bool stat(const std::string& path, std::function<void(Error e)> callback);

bool fstat(const std::string& path, std::function<void(Error e)> callback);
#endif
} /* namespace fs */

class File {
  public:
    static bool read(const std::string &path, std::function<void(const std::string &str, Error e)> callback);

    static bool
    write(const std::string &path, const std::string &str, std::function<void(int nwritten, Error e)> callback);
};

} /* namespace native */

#endif /* __NATIVE_FS_H__ */
