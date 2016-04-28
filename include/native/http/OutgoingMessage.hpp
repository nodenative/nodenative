#ifndef __NATIVE_HTTP_OUTGOINGMESSAGE_HPP__
#define __NATIVE_HTTP_OUTGOINGMESSAGE_HPP__

#include "../text.hpp"

#include <string>
#include <map>
#include <memory>

namespace native {

namespace http {

class OutgoingMessage
{
protected:
    OutgoingMessage() = delete;
    OutgoingMessage(const bool iIsRequest);
    virtual Future<void> send(const std::string& data) = 0;
public:
    virtual ~OutgoingMessage();

    void setStatus(int status_code);
    void setHeader(const std::string& key, const std::string& value);
    bool getHeader(const std::string& key, std::string& value);
    bool removeHeader(const std::string& key);
    static std::string GetStatusText(int status);

    std::string getMessageHeaderRaw() const;

    Future<void> write(const std::string& str);
    virtual Future<void> end(const std::string& body);
    virtual Future<void> end();

protected:
    const bool _isRequest;
    std::map<std::string, std::string, native::text::ci_less> _headers;
    int _statusCode;
    bool _headerSent;
    bool _closed;

    bool _last;
    bool _chunkedEncoding;
    bool _useChunkedEncodingByDefault;
    bool _sendDate;
    bool _shouldKeepAlive;

    int _contentLength;
    bool _hasBody;
    std::string _trailer;
};

} /* namespace http */
} /* namespace native */

#endif /* __NATIVE_HTTP_OUTGOINGMESSAGE_HPP__ */
