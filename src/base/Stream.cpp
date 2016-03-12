#include "native/base/Stream.hpp"
#include "native/async.hpp"
#include "native/async.ipp"
#include "native/async/RequestPromise.hpp"

namespace native {
namespace base {

bool Stream::listen(std::function<void(native::Error)> callback, int backlog)
{
    NNATIVE_ASSERT(callback);
    _listenCallback = callback;
    return uv_listen(get<uv_stream_t>(), backlog, [](uv_stream_t* s, int status) {
        Stream* currptr = static_cast<Stream*>(s->data);
        currptr->_listenCallback((status != 0)?Error(status):Error());
    }) == 0;
}

bool Stream::accept(std::shared_ptr<Stream> client)
{
    return uv_accept(get<uv_stream_t>(), client->get<uv_stream_t>()) == 0;
}

bool Stream::readStart(std::function<void(const char* buf, ssize_t len)> callback)
{
    NNATIVE_ASSERT(!_readCb);

    int res = uv_read_start(get<uv_stream_t>(),
        [](uv_handle_t* s, size_t suggested_size, uv_buf_t* buf){
            Stream * thisPtr = static_cast<Stream*>(s->data);
            auto size = std::max(suggested_size, thisPtr->_maxAllocSize);
            buf->base = new char[size];
            buf->len = size;
            if((buf->base == NULL) && (buf->len > 0))
            {
                NNATIVE_ASSERT(0);
                //TODO: Error
            }
        },
        [](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf){
            Stream * thisPtr = static_cast<Stream*>(s->data);
            std::unique_ptr<char[]> bufData(buf->base);
            if(nread < 0)
            {
                NNATIVE_ASSERT(nread == UV_EOF);
                thisPtr->_readCb(nullptr, nread);
            }
            else if(nread >= 0)
            {
                thisPtr->_readCb(bufData.get(), nread);
            }
        });

    if(res == 0) {
        _readCb = callback;
    }

    return res == 0;
}

bool Stream::readStop()
{
    return uv_read_stop(get<uv_stream_t>()) == 0;
}

// TODO: implement read2_start()

Future<void> Stream::write(const uv_buf_t* bufs, const size_t count, uv_stream_t* iSendHandle)
{
    std::unique_ptr<RequestPromise<void, uv_write_t>> reqInstance(new RequestPromise<void, uv_write_t>(_loop, getInstanceStream()));

    int res;
    if(iSendHandle == nullptr) {
        res = uv_write(&reqInstance->_req, get<uv_stream_t>(), bufs, count, &Stream::AfterWrite);
    } else {
        res = uv_write2(&reqInstance->_req, get<uv_stream_t>(), bufs, count, iSendHandle, &Stream::AfterWrite);
    }

    Future<void> future = reqInstance->_promise.getFuture();

    if(res != 0) {
        PROMISE_REJECT(reqInstance->_promise, "Error in uv_write, status: " << res);
        return future;
    }

    reqInstance.release();
    return future;
}

void Stream::AfterWrite(uv_write_t* req, int status)
{
    std::unique_ptr<RequestPromise<void, uv_write_t>> reqInstance(static_cast<RequestPromise<void, uv_write_t>*>(req->data));
    Error curError(status);
    if(!curError) {
        reqInstance->_promise.resolve();
    } else {
        reqInstance->_promise.reject(curError.str());
    }
}

Future<void> Stream::write(const char* buf, const int len, uv_stream_t* iSendHandle)
{
    uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf), static_cast<size_t>(len) } };
    return write(bufs, static_cast<size_t>(1U), iSendHandle);
}

Future<void> Stream::write(const std::string& buf, uv_stream_t* iSendHandle)
{
    uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf.c_str()), buf.length()} };
    return write(bufs, static_cast<size_t>(1U), iSendHandle);
}

Future<void> Stream::write(const std::vector<char>& buf, uv_stream_t* iSendHandle)
{
    uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(&buf[0]), buf.size() } };
    return write(bufs, static_cast<size_t>(1U), iSendHandle);
}

Future<void> Stream::shutdown()
{
    std::unique_ptr<RequestPromise<void, uv_shutdown_t>> reqInstance(new RequestPromise<void, uv_shutdown_t>(_loop, getInstanceStream()));

    const int res = uv_shutdown(&reqInstance->_req, get<uv_stream_t>(), [](uv_shutdown_t* req, int status) {
            std::unique_ptr<RequestPromise<void, uv_shutdown_t>> reqInstance(static_cast<RequestPromise<void, uv_shutdown_t>*>(req->data));
            if(0 == status) {
                reqInstance->_promise.resolve();
            } else {
                PROMISE_REJECT(reqInstance->_promise, "Returned non zero status " << status);
            }
        });

    Future<void> future = reqInstance->_promise.getFuture();

    if(res != 0) {
        PROMISE_REJECT(reqInstance->_promise, "Error in uv_shutdown, status: " << res);
        return future;
    }

    reqInstance.release();
    return future;
}

} /* namespace base */
} /* namespace native */
