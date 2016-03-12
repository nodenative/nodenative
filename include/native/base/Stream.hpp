#ifndef __NATIVE_BASE_STREAM_HPP__
#define __NATIVE_BASE_STREAM_HPP__

#include "base_utils.hpp"
#include "../Error.hpp"
#include "Handle.hpp"

#include <algorithm>

namespace native {

namespace base {

class Stream : public Handle
{
public:
    Stream() = delete;
    Stream(std::shared_ptr<Loop> iLoop)
        : Handle(iLoop),
        _maxAllocSize(0)
    { }

    bool listen(std::function<void(native::Error)> callback, int backlog=128);

    bool accept(std::shared_ptr<Stream> client);

    bool readStart(std::function<void(const char* buf, ssize_t len)> callback);
    bool readStop();

    // TODO: implement read2_start()

    Future<void> write(const uv_buf_t *bufs, const size_t count, uv_stream_t* iSendHandle);
    Future<void> write(const char* buf, const int len, uv_stream_t* iSendHandle = nullptr);
    Future<void> write(const std::string& buf, uv_stream_t* iSendHandle = nullptr);
    Future<void> write(const std::vector<char>& buf, uv_stream_t* iSendHandle = nullptr);

    Future<void> shutdown();

    void setMaxAllocSize(size_t iSize) {
        _maxAllocSize = iSize;
    }

    std::shared_ptr<Stream> getInstanceStream() {
        return std::static_pointer_cast<Stream>(Handle::getInstanceHandle());
    }

private:
    static void AfterWrite(uv_write_t* req, int status);

    size_t _maxAllocSize;
    std::function<void(const char* buf, ssize_t len)> _readCb;
    std::function<void(Error)> _listenCallback;
};

} /* namespace base */
} /* namespace native */

#endif /* __NATIVE_BASE_STREAM_HPP__ */
