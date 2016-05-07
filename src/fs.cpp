#include "native/fs.hpp"
#include "native/async/RequestPromise.hpp"

namespace {
/*
struct rte_context
{
    fs::file_handle file;
    const static int buflen = 32;
    char buf[buflen];
    std::string result;
};

template<typename callback_t>
void rte_cb(uv_fs_t* req)
{
    assert(req->fs_type == UV_FS_READ);

    auto ctx =
reinterpret_cast<rte_context*>(callbacks::get_data<callback_t>(req->data, 0));
    if(req->result < 0)
    {
        // system Error
        Error err(req->result);
        invoke_from_req<callback_t>(req, std::string(), err);
        delete_req_template<callback_t, rte_context>(req);
    }
    else if(req->result == 0)
    {
        // EOF
        Error err;
        invoke_from_req<callback_t>(req, ctx->result, err);
        delete_req_template<callback_t, rte_context>(req);
    }
    else
    {
        ctx->result.append(std::string(ctx->buf, req->result));

        uv_fs_req_cleanup(req);
        const uv_buf_t iov = uv_buf_init(ctx->buf, rte_context::buflen);

        Error err(uv_fs_read(uv_default_loop(), req, ctx->file, &iov, 1,
ctx->result.length(), rte_cb<callback_t>));
        if(err)
        {
            invoke_from_req<callback_t>(req, std::string(), err);
            delete_req_template<callback_t, rte_context>(req);
        }
    }
}
*/
} /* namespace */

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

int stringToFlags(const std::string &flagStr) {
    static std::map<std::string, int> flagsMap;
    if (flagsMap.empty()) {
        flagsMap["r"] = O_RDONLY;
        flagsMap["rs"] = flagsMap["sr"] = O_RDONLY | O_SYNC;
        flagsMap["rw"] = flagsMap["r+"] = O_RDWR;
        flagsMap["rs+"] = flagsMap["sr+"] = O_RDWR | O_SYNC;
        flagsMap["w"] = O_TRUNC | O_CREAT | O_WRONLY;
        flagsMap["wx"] = flagsMap["xw"] = O_TRUNC | O_CREAT | O_WRONLY | O_EXCL;
        flagsMap["w+"] = O_TRUNC | O_CREAT | O_RDWR;
        flagsMap["wx+"] = flagsMap["xw+"] = O_TRUNC | O_CREAT | O_RDWR | O_EXCL;
        flagsMap["a"] = O_APPEND | O_CREAT | O_WRONLY;
        flagsMap["ax"] = flagsMap["xa"] = O_APPEND | O_CREAT | O_WRONLY | O_EXCL;
        flagsMap["a+"] = O_APPEND | O_CREAT | O_RDWR;
        flagsMap["ax+"] = flagsMap["xa+"] = O_APPEND | O_CREAT | O_RDWR | O_EXCL;
    }

    std::map<std::string, int>::const_iterator it = flagsMap.find(flagStr);

    if (it == flagsMap.end()) {
        return 0;
    }

    return it->second;
}

Future<file_handle> open(const std::string &path, const int flags, const int mode) {
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();
    auto reqInstance = RequestPromise<native::fs::file_handle, uv_fs_t>::Create(loopInst);
    Error err;
    if ((err = uv_fs_open(loopInst->get(), &reqInstance->_req, path.c_str(), flags, mode, [](uv_fs_t *req) {
             NNATIVE_ASSERT(req->fs_type == UV_FS_OPEN);
             std::unique_ptr<RequestPromise<native::fs::file_handle, uv_fs_t>> reqInstance(
                 static_cast<RequestPromise<native::fs::file_handle, uv_fs_t> *>(req->data));

             if (req->result < 0) {
                 Error err(req->result);
                 reqInstance->_promise.reject(err.str());
             } else {
                 reqInstance->_promise.resolve(req->result);
             }

             uv_fs_req_cleanup(req);
         }))) {
        reqInstance->_promise.reject(err.str());
        uv_fs_req_cleanup(&reqInstance->_req);
    }

    return reqInstance.release()->_promise.getFuture();
}

Future<file_handle> open(const std::string &path, const std::string &flags, const int mode) {
    return open(path, stringToFlags(flags), mode);
}

file_handle openSync(const std::string &path, const int flags, const int mode) {
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();
    uv_fs_t req;
    file_handle fd;
    if ((fd = uv_fs_open(loopInst->get(), &req, path.c_str(), flags, mode, nullptr)) < 0) {
        uv_fs_req_cleanup(&req);
        Error err(fd);
        throw err;
    }

    uv_fs_req_cleanup(&req);

    return fd;
}

file_handle openSync(const std::string &path, const std::string &flags, const int mode) {
    return openSync(path, stringToFlags(flags), mode);
}

Future<std::shared_ptr<std::string>> read(const file_handle fd, const size_t len, const off_t offset) {
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();
    std::shared_ptr<std::string> bufInst(new std::string());
    bufInst->resize(len);
    auto reqInstance =
        RequestPromiseInstance<std::shared_ptr<std::string>, uv_fs_t, std::string>::Create(loopInst, bufInst);
    const uv_buf_t iov = uv_buf_init(const_cast<char *>(bufInst->c_str()), len);
    Error err;
    if ((err = uv_fs_read(loopInst->get(), &reqInstance->_req, fd, &iov, 1, offset, [](uv_fs_t *req) {
             NNATIVE_ASSERT(req->fs_type == UV_FS_READ);
             std::unique_ptr<RequestPromiseInstance<std::shared_ptr<std::string>, uv_fs_t, std::string>> reqInstance(
                 static_cast<RequestPromiseInstance<std::shared_ptr<std::string>, uv_fs_t, std::string> *>(req->data));

             if (req->result < 0) {
                 Error err(req->result);
                 reqInstance->_promise.reject(err.str());
             } else {
                 reqInstance->_instance->resize(req->result);
                 reqInstance->_promise.resolve(reqInstance->_instance);
             }

             uv_fs_req_cleanup(req);
         }))) {
        reqInstance->_promise.reject(err.str());
        uv_fs_req_cleanup(&reqInstance->_req);
    }

    return reqInstance.release()->_promise.getFuture();
}

std::shared_ptr<std::string> readSync(const file_handle fd, const size_t len, const off_t offset) {
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();
    std::shared_ptr<std::string> bufInst(new std::string());
    bufInst->resize(len);
    const uv_buf_t iov = uv_buf_init(const_cast<char *>(bufInst->c_str()), len);
    uv_fs_t req;
    int res;
    if ((res = uv_fs_read(loopInst->get(), &req, fd, &iov, 1, offset, nullptr)) < 0) {
        Error err(res);
        uv_fs_req_cleanup(&req);
        throw err;
    }

    uv_fs_req_cleanup(&req);

    return bufInst;
}

Future<int> write(const file_handle fd, const char *buf, const size_t len, const off_t offset) {
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();
    // TODO: remove const_cast<> !!
    const uv_buf_t iov = uv_buf_init(const_cast<char *>(buf), len);
    auto reqInstance = RequestPromise<int, uv_fs_t>::Create(loopInst);

    Error err;
    if ((err = uv_fs_write(loopInst->get(), &reqInstance->_req, fd, &iov, 1, offset, [](uv_fs_t *req) {
             NNATIVE_ASSERT(req->fs_type == UV_FS_WRITE);
             std::unique_ptr<RequestPromise<int, uv_fs_t>> reqInstance(
                 static_cast<RequestPromise<int, uv_fs_t> *>(req->data));

             if (req->result < 0) {
                 Error err(req->result);
                 reqInstance->_promise.reject(err.str());
             } else {
                 reqInstance->_promise.resolve(req->result);
             }

             uv_fs_req_cleanup(req);
         }))) {
        reqInstance->_promise.reject(err.str());
        uv_fs_req_cleanup(&reqInstance->_req);
    }

    return reqInstance.release()->_promise.getFuture();
}

Future<int> write(const file_handle fd, const std::string &buf, const off_t offset) {
    return write(fd, buf.c_str(), buf.size(), offset);
}

int writeSync(const file_handle fd, const char *buf, const size_t len, const off_t offset) {
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();
    // TODO: remove const_cast<> !!
    const uv_buf_t iov = uv_buf_init(const_cast<char *>(buf), len);
    uv_fs_t req;
    int res;
    if ((res = uv_fs_write(loopInst->get(), &req, fd, &iov, 1, offset, nullptr)) < 0) {
        Error err(res);
        uv_fs_req_cleanup(&req);
        throw err;
    }

    uv_fs_req_cleanup(&req);

    return res;
}

int writeSync(const file_handle fd, const std::string &buf, const off_t offset) {
    return writeSync(fd, buf.c_str(), buf.size(), offset);
}

/*
bool read_to_end(file_handle fd, std::function<void(const std::string& str,
Error e)> callback)
{
    auto ctx = new internal::rte_context;
    ctx->file = fd;
    auto req = internal::create_req(callback, ctx);
    const uv_buf_t iov = uv_buf_init(ctx->buf, internal::rte_context::buflen);

    if(uv_fs_read(uv_default_loop(), req, fd, &iov, 1, 0,
internal::rte_cb<decltype(callback)>)) {
        // failed to initiate uv_fs_read()
        internal::delete_req_template<decltype(callback),
internal::rte_context>(req);
        return false;
    }
    return true;
}
*/
#define UV_FS_METHOD_CALL_PROMISE_VOID(method, expectedFsType, ...)                                                    \
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();                                               \
    auto reqInstance = RequestPromise<void, uv_fs_t>::Create(loopInst);                                                \
    Error err;                                                                                                         \
    if ((err = uv_fs_##method(loopInst->get(), &reqInstance->_req, __VA_ARGS__, [](uv_fs_t *req) {                     \
             NNATIVE_ASSERT(req->fs_type == expectedFsType);                                                           \
             std::unique_ptr<RequestPromise<void, uv_fs_t>> reqInstance(                                               \
                 static_cast<RequestPromise<void, uv_fs_t> *>(req->data));                                             \
                                                                                                                       \
             if (req->result < 0) {                                                                                    \
                 Error err(req->result);                                                                               \
                 reqInstance->_promise.reject(err.str());                                                              \
             } else {                                                                                                  \
                 reqInstance->_promise.resolve();                                                                      \
             }                                                                                                         \
             uv_fs_req_cleanup(req);                                                                                   \
         }))) {                                                                                                        \
        reqInstance->_promise.reject(err.str());                                                                       \
        uv_fs_req_cleanup(&reqInstance->_req);                                                                         \
    }                                                                                                                  \
    return reqInstance.release()->_promise.getFuture();

#define UV_FS_METHOD_SYNCCALL_VOID(method, expectedFsType, ...)                                                        \
    std::shared_ptr<Loop> loopInst = Loop::GetInstanceOrCreateDefault();                                               \
    uv_fs_t req;                                                                                                       \
    Error err;                                                                                                         \
    if ((err = uv_fs_##method(loopInst->get(), &req, __VA_ARGS__, nullptr))) {                                         \
        uv_fs_req_cleanup(&req);                                                                                       \
        throw err;                                                                                                     \
    }                                                                                                                  \
    uv_fs_req_cleanup(&req);

Future<void> access(const std::string &path, int mode) {
    UV_FS_METHOD_CALL_PROMISE_VOID(access, UV_FS_MKDIR, path.c_str(), mode);
}

void accessSync(const std::string &path, int mode) {
    UV_FS_METHOD_SYNCCALL_VOID(access, UV_FS_MKDIR, path.c_str(), mode);
}

Future<void> close(const file_handle fd) { UV_FS_METHOD_CALL_PROMISE_VOID(close, UV_FS_CLOSE, fd); }

void closeSync(const file_handle fd) { UV_FS_METHOD_SYNCCALL_VOID(close, UV_FS_CLOSE, fd); }

Future<void> unlink(const std::string &path) { UV_FS_METHOD_CALL_PROMISE_VOID(unlink, UV_FS_UNLINK, path.c_str()); }

void unlinkSync(const std::string &path) { UV_FS_METHOD_SYNCCALL_VOID(unlink, UV_FS_UNLINK, path.c_str()); }

Future<void> mkdir(const std::string &path, int mode) {
    UV_FS_METHOD_CALL_PROMISE_VOID(mkdir, UV_FS_MKDIR, path.c_str(), mode);
}

void mkdirSync(const std::string &path, int mode) {
    UV_FS_METHOD_SYNCCALL_VOID(mkdir, UV_FS_MKDIR, path.c_str(), mode);
}

Future<void> rmdir(const std::string &path, std::function<void(Error e)> callback) {
    UV_FS_METHOD_CALL_PROMISE_VOID(rmdir, UV_FS_RMDIR, path.c_str());
}

void rmdirSync(const std::string &path, std::function<void(Error e)> callback) {
    UV_FS_METHOD_SYNCCALL_VOID(rmdir, UV_FS_RMDIR, path.c_str());
}

Future<void> rename(const std::string &path, const std::string &new_path) {
    UV_FS_METHOD_CALL_PROMISE_VOID(rename, UV_FS_RENAME, path.c_str(), new_path.c_str());
}

void renameSync(const std::string &path, const std::string &new_path) {
    UV_FS_METHOD_SYNCCALL_VOID(rename, UV_FS_RENAME, path.c_str(), new_path.c_str());
}

Future<void> chmod(const std::string &path, int mode) {
    UV_FS_METHOD_CALL_PROMISE_VOID(chmod, UV_FS_CHMOD, path.c_str(), mode);
}

void chmodSync(const std::string &path, int mode) {
    UV_FS_METHOD_SYNCCALL_VOID(chmod, UV_FS_CHMOD, path.c_str(), mode);
}

Future<void> fchmod(const file_handle fd, int mode) { UV_FS_METHOD_CALL_PROMISE_VOID(fchmod, UV_FS_CHMOD, fd, mode); }

void fchmodSync(const file_handle fd, int mode) { UV_FS_METHOD_SYNCCALL_VOID(fchmod, UV_FS_CHMOD, fd, mode); }

Future<void> chown(const std::string &path, int uid, int gid) {
    UV_FS_METHOD_CALL_PROMISE_VOID(chown, UV_FS_CHOWN, path.c_str(), uid, gid);
}

void chownSync(const std::string &path, int uid, int gid) {
    UV_FS_METHOD_SYNCCALL_VOID(chown, UV_FS_CHOWN, path.c_str(), uid, gid);
}

#undef UV_FS_METHOD_CALL_PROMISE_VOID
#undef UV_FS_METHOD_SYNCCALL_VOID

#if 0
Future<void> readdir(const std::string& path, int flags, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_readdir(uv_default_loop(), req, path.c_str(), flags, [](uv_fs_t* req){
        NNATIVE_ASSERT(req->fs_type == UV_FS_READDIR);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

Future<void> stat(const std::string& path, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_stat(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        NNATIVE_ASSERT(req->fs_type == UV_FS_STAT);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool fstat(const std::string& path, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_fstat(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        NNATIVE_ASSERT(req->fs_type == UV_FS_FSTAT);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}
#endif

/*
bool file::read(const std::string& path, std::function<void(const std::string&
str, Error e)> callback)
{
    return fs::open(path.c_str(), fs::read_only, 0, [=](fs::file_handle fd,
Error e) {
        if(e)
        {
            callback(std::string(), e);
        }
        else
        {
            if(!fs::read_to_end(fd, callback))
            {
                // failed to initiate read_to_end()
                //TODO: this should not happen for async (callback provided).
Temporary return unknown Error. To resolve this.
                //callback(std::string(),
Error(uv_last_error(uv_default_loop())));
                callback(std::string(), Error(UV__UNKNOWN));
            }
        }
    });
}

bool file::write(const std::string& path, const std::string& str,
std::function<void(int nwritten, Error e)> callback)
{
    return fs::open(path.c_str(), fs::write_only|fs::create, 0664,
[=](fs::file_handle fd, Error e) {
        if(e)
        {
            callback(0, e);
        }
        else
        {
            if(!fs::write(fd, str.c_str(), str.length(), 0, callback))
            {
                // failed to initiate read_to_end()
                //TODO: this should not happen for async (callback provided).
Temporary return unknown Error. To resolve this.
                //callback(0, Error(uv_last_error(uv_default_loop())));
                callback(0, Error(UV__UNKNOWN));
            }
        }
    });
}
};

*/

} // namespace fs
} // namespace native
