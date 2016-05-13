#ifndef __NATIVE_FS_H__
#define __NATIVE_FS_H__

#include "Error.hpp"
#include "base.hpp"
#include <algorithm>
#include <fcntl.h>
#include <functional>

namespace native {
/**
 * @defgroup fs File System
 * File System methods
 * @{
 */

// TODO: implement functions that accept Loop pointer as extra argument.
namespace fs {

typedef uv_file file_handle;
typedef uv_dirent_t DirEnt;

struct Stats {
  bool isDirectory();
  bool isFile();
  bool isBlockDevice();
  bool isCharacterDevice();
  bool isSymbolicLink();
  bool isFIFO();
  bool isSocket();

  Stats &operator=(const uv_stat_t stats);
  Stats &operator=(const uv_stat_t *stats);

  uint64_t dev;
  uint64_t mode;
  uint64_t nlink;
  uint64_t uid;
  uint64_t gid;
  uint64_t rdev;
  uint64_t ino;
  uint64_t size;
  uint64_t blksize;
  uint64_t blocks;
  uint64_t flags;
  uint64_t gen;
  uv_timespec_t atim;
  uv_timespec_t mtim;
  uv_timespec_t ctim;
  uv_timespec_t birthtim;

private:
  bool checkModeproperty(const int iFlag);
};

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

Future<void> access(const std::string &path, const int mode);
void accessSync(const std::string &path, const int mode);

Future<void> close(const file_handle fd);
void closeSync(const file_handle fd);

Future<void> unlink(const std::string &path);
void unlinkSync(const std::string &path);

Future<void> mkdir(const std::string &path, const int mode);
void mkdirSync(const std::string &path, const int mode);

Future<std::string> mkdtemp(const std::string &prefix);
std::string mkdtempSync(const std::string &prefix);

Future<void> rmdir(const std::string &path);
void rmdirSync(const std::string &path);

Future<void> rename(const std::string &iOldPath, const std::string &iNewPath);
void renameSync(const std::string &iOldPath, const std::string &iNewPath);

Future<void> chmod(const std::string &path, const int mode);
void chmodSync(const std::string &path, const int mode);

Future<void> fchmod(const file_handle fd, const int mode);
void fchmodSync(const file_handle fd, const int mode);

Future<void> chown(const std::string &path, const int uid, const int gid);
void chownSync(const std::string &path, const int uid, const int gid);

/**
 * Asynchronous <a href="http://man7.org/linux/man-pages/man2/fchown.2.html">fchown(2)</a>. Return a Future void object
 * @param fd  file description
 * @param uid user ID
 * @param gid Group ID
 * @return Future<void>
 */
Future<void> fchown(const file_handle fd, const int uid, const int gid);

/**
 * Synchronous fchown(2).
 * @param fd  file description
 * @param uid user ID
 * @param gid Group ID
 */
void fchownSync(const file_handle fd, const int uid, const int gid);

/**
 * Asynchronous <a href="http://man7.org/linux/man-pages/man2/fdatasync.2.html">fdatasync(2)</a>. Return a Future void
 * object
 * @param fd  file description
 * @return Future<void>
 */
Future<void> fdatasync(const file_handle fd);

/**
 * Synchronous fdatasync(2).
 * @param fd  file description
 */
void fdatasyncSync(const file_handle fd);

/**
 * Asynchronous <a href="http://man7.org/linux/man-pages/man2/fsync.2.html">fsync(2)</a>. Return a Future void
 * object
 * @param fd  file description
 * @return Future<void>
 */
Future<void> fsync(const file_handle fd);

/**
 * Synchronous fsync(2).
 * @param fd  file description
 */
void fsyncSync(const file_handle fd);

/**
 * Asynchronous <a href="http://man7.org/linux/man-pages/man2/ftruncate.2.html">ftruncate(2)</a>. Return a Future void
 * object
 * @param fd  file description
 * @param len length
 * @return Future<void>
 */
Future<void> ftruncate(const file_handle fd, const size_t len);

/**
 * Synchronous ftruncate(2).
 * @param fd  file description
 * @param len length
 */
void ftruncateSync(const file_handle fd, const size_t len);

Future<void> futime(const file_handle fd, const double atime, const double mtime);
void futimeSync(const file_handle fd, const double atime, const double mtime);

Future<void> utime(const std::string &path, const double atime, const double mtime);
void utimeSync(const std::string &path, const double atime, const double mtime);

Future<void> link(const std::string &srcpath, const std::string &dstpath);
void linkSync(const std::string &srcpath, const std::string &dstpath);

Future<void> symlink(const std::string &srcpath, const std::string &dstpath, const std::string &type = "file");
void symlinkSync(const std::string &srcpath, const std::string &dstpath, const std::string &type = "file");

/**
 * Asynchronous `readdir(3)`. Reads the contents of a directory.
 */
Future<std::shared_ptr<std::vector<DirEnt>>> readdir(const std::string &path, const int flags = 0);
/**
 * Synchronous `readdir(3)`. Returns an vector instance of filenames and types excluding '.' and '..'.
 */
std::shared_ptr<std::vector<DirEnt>> readdirSync(const std::string &path, const int flags = 0);

/**
 * Asynchronous `readdir(3)`. Reads the contents of a directory.
 */
Future<std::shared_ptr<std::string>> readFile(const std::string &path, const int flags = 0);
Future<std::shared_ptr<std::string>> readFile(const file_handle fd, const int flags = 0);
/**
 * Synchronous `readdir(3)`. Returns an vector instance of filenames and types excluding '.' and '..'.
 */
std::shared_ptr<std::string> readFileSync(const std::string &path, const int flags = 0);
std::shared_ptr<std::string> readFileSync(const file_handle fd, const int flags = 0);

Future<std::string> readlink(const std::string &path);
std::string readlinkSync(const std::string &path);

/**
 * Equivalent to realpath(3) on Unix. Windows uses GetFinalPathNameByHandle().
 * @note This function is not implemented on Windows XP and Windows Server 2003. On these systems, UV_ENOSYS is
 * returned.
 */
Future<std::string> realpath(const std::string &path);
std::string realpathSync(const std::string &path);

Future<std::shared_ptr<Stats>> stat(const std::string &path);
Future<std::shared_ptr<Stats>> fstat(const file_handle fd);
Future<std::shared_ptr<Stats>> lstat(const std::string &path);

std::shared_ptr<Stats> statSync(const std::string &path);
std::shared_ptr<Stats> fstatSync(const file_handle fd);
std::shared_ptr<Stats> lstatSync(const std::string &path);

Future<bool> exists(const std::string &path);
bool existsSync(const std::string &path);

} /* namespace fs */

class File {
public:
  static bool read(const std::string &path, std::function<void(const std::string &str, Error e)> callback);

  static bool
  write(const std::string &path, const std::string &str, std::function<void(int nwritten, Error e)> callback);
};

/**
 * @}  // end of fs
 */

} /* namespace native */

#endif /* __NATIVE_FS_H__ */
