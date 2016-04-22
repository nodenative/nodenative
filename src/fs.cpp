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

    auto ctx = reinterpret_cast<rte_context*>(callbacks::get_data<callback_t>(req->data, 0));
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

        Error err(uv_fs_read(uv_default_loop(), req, ctx->file, &iov, 1, ctx->result.length(), rte_cb<callback_t>));
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
    if(flagsMap.empty()) {
        flagsMap["r"]                     = O_RDONLY;
        flagsMap["rs"] = flagsMap["sr"]   = O_RDONLY | O_SYNC;
        flagsMap["rw"] = flagsMap["r+"]   = O_RDWR;
        flagsMap["rs+"] = flagsMap["sr+"] = O_RDWR | O_SYNC;
        flagsMap["w"]                     = O_TRUNC | O_CREAT | O_WRONLY;
        flagsMap["wx"] = flagsMap["xw"]   = O_TRUNC | O_CREAT | O_WRONLY | O_EXCL;
        flagsMap["w+"]                    = O_TRUNC | O_CREAT | O_RDWR;
        flagsMap["wx+"] = flagsMap["xw+"] = O_TRUNC | O_CREAT | O_RDWR | O_EXCL;
        flagsMap["a"]                     = O_APPEND | O_CREAT | O_WRONLY;
        flagsMap["ax"] = flagsMap["xa"]   = O_APPEND | O_CREAT | O_WRONLY | O_EXCL;
        flagsMap["a+"]                    = O_APPEND | O_CREAT | O_RDWR;
        flagsMap["ax+"] = flagsMap["xa+"] = O_APPEND | O_CREAT | O_RDWR | O_EXCL;
    }

    std::map<std::string, int>::const_iterator it = flagsMap.find(flagStr);

    if(it == flagsMap.end()) {
        return 0;
    }

    return it->second;
}

Future<file_handle> open(const std::string& path, int flags, int mode)
{
    auto reqInstance = RequestPromise<native::fs::file_handle, uv_fs_t>::Create(Loop::);
    Error err;
    if((err = uv_fs_open(uv_default_loop(), req, path.c_str(), flags, mode, [](uv_fs_t* req) {
        NNATIVE_ASSERT(req->fs_type == UV_FS_OPEN);
        std::unique_ptr<RequestPromise<native::fs::file_handle, uv_fs_t>> reInstance(static_cast<RequestPromise<native::fs::file_handle, uv_fs_t>*>(req->data));

        if(req->result < 0) {
            Error err(req->result);
            reInstance->_promise.reject(err.str());
            return;
        }

        reInstance->_promise.resolve(req->result);
    }))) {
        RequestPromise->_promise.reject(err.str());
    }

    return RequestPromise->_promise.getFuture();
}

/*
bool read(file_handle fd, size_t len, off_t offset, std::function<void(const std::string& str, Error e)> callback)
{
    auto buf = new char[len];
    auto req = internal::create_req(callback, buf);
    const uv_buf_t iov = uv_buf_init(buf, len);
    if(uv_fs_read(uv_default_loop(), req, fd, &iov, 1, offset, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_READ);

        if(req->result < 0)
        {
            // system Error
            internal::invoke_from_req<decltype(callback)>(req, std::string(), Error(req->result));
        }
        else if(req->result == 0)
        {
            // EOF
            internal::invoke_from_req<decltype(callback)>(req, std::string(), Error(UV_EOF));
        }
        else
        {
            auto buf = internal::get_data_from_req<decltype(callback), char>(req);
            internal::invoke_from_req<decltype(callback)>(req, std::string(buf, req->result), Error());
        }

        internal::delete_req_arr_data<decltype(callback), char>(req);
    })) {
        // failed to initiate uv_fs_read()
        internal::delete_req_arr_data<decltype(callback), char>(req);
        return false;
    }
    return true;
}

bool write(file_handle fd, const char* buf, size_t len, off_t offset, std::function<void(int nwritten, Error e)> callback)
{
    auto req = internal::create_req(callback);
    const uv_buf_t iov = uv_buf_init(const_cast<char*>(buf), len);

    // TODO: const_cast<> !!
    if(uv_fs_write(uv_default_loop(), req, fd, &iov, 1, offset, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_WRITE);

        if(req->result)
        {
            internal::invoke_from_req<decltype(callback)>(req, 0, Error(req->result));
        }
        else
        {
            internal::invoke_from_req<decltype(callback)>(req, req->result, Error());
        }

        internal::delete_req(req);
    })) {
        // failed to initiate uv_fs_write()
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool read_to_end(file_handle fd, std::function<void(const std::string& str, Error e)> callback)
{
    auto ctx = new internal::rte_context;
    ctx->file = fd;
    auto req = internal::create_req(callback, ctx);
    const uv_buf_t iov = uv_buf_init(ctx->buf, internal::rte_context::buflen);

    if(uv_fs_read(uv_default_loop(), req, fd, &iov, 1, 0, internal::rte_cb<decltype(callback)>)) {
        // failed to initiate uv_fs_read()
        internal::delete_req_template<decltype(callback), internal::rte_context>(req);
        return false;
    }
    return true;
}

bool close(file_handle fd, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_close(uv_default_loop(), req, fd, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_CLOSE);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool unlink(const std::string& path, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_unlink(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_UNLINK);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool mkdir(const std::string& path, int mode, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_mkdir(uv_default_loop(), req, path.c_str(), mode, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_MKDIR);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool rmdir(const std::string& path, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_rmdir(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_RMDIR);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool rename(const std::string& path, const std::string& new_path, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_rename(uv_default_loop(), req, path.c_str(), new_path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_RENAME);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool chmod(const std::string& path, int mode, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_chmod(uv_default_loop(), req, path.c_str(), mode, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_CHMOD);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool chown(const std::string& path, int uid, int gid, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_chown(uv_default_loop(), req, path.c_str(), uid, gid, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_CHOWN);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

#if 0
bool readdir(const std::string& path, int flags, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_readdir(uv_default_loop(), req, path.c_str(), flags, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_READDIR);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool stat(const std::string& path, std::function<void(Error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_stat(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_STAT);
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
        assert(req->fs_type == UV_FS_FSTAT);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0?Error(req->result):Error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}
#endif
}

class file
{
public:
static bool read(const std::string& path, std::function<void(const std::string& str, Error e)> callback)
{
    return fs::open(path.c_str(), fs::read_only, 0, [=](fs::file_handle fd, Error e) {
        if(e)
        {
            callback(std::string(), e);
        }
        else
        {
            if(!fs::read_to_end(fd, callback))
            {
                // failed to initiate read_to_end()
                //TODO: this should not happen for async (callback provided). Temporary return unknown Error. To resolve this.
                //callback(std::string(), Error(uv_last_error(uv_default_loop())));
                callback(std::string(), Error(UV__UNKNOWN));
            }
        }
    });
}

static bool write(const std::string& path, const std::string& str, std::function<void(int nwritten, Error e)> callback)
{
    return fs::open(path.c_str(), fs::write_only|fs::create, 0664, [=](fs::file_handle fd, Error e) {
        if(e)
        {
            callback(0, e);
        }
        else
        {
            if(!fs::write(fd, str.c_str(), str.length(), 0, callback))
            {
                // failed to initiate read_to_end()
                //TODO: this should not happen for async (callback provided). Temporary return unknown Error. To resolve this.
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
