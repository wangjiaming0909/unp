#ifndef DOWNLOADERGETFILEINFOHANDLER_H
#define DOWNLOADERGETFILEINFOHANDLER_H

#include "DownloaderHandler.h"

namespace examples {


class DownloaderGetFileInfoHandler : public DownloaderHandler
{
public:
    DownloaderGetFileInfoHandler(reactor::Reactor& r, MessageSetupCallback_t&& callback, bool isSSL)
        : DownloaderHandler{r, std::forward<MessageSetupCallback_t>(callback), isSSL}
    {
    }

    virtual int onHeadersComplete(size_t len) override;

    virtual int onBody(const char* buf, size_t len) override;

    uint64_t getContentLength() const {return codec_.contentLength();}

    const std::string* getFileName() const
    {
        return codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_CONTENT_DISPOSITION);
    }

    bool succeed() const {return succeed_;}

    const http::Http1xCodec& getCodec() const {return codec_;}

    const http::HttpMessage& getResponseMessage() const {return codec_.message();}

    int getStatus() const {return codec_.status();}

    const std::string* getResponseHeaderValue(http::HttpHeaderCode code) const
    {
        return codec_.message().getHeaderValue(code);
    }

    int handle_input(int handle) override
    {
        DownloaderHandler::handle_input(handle);

        if(isShouldClose_)
        {
            return -1;
        }
        return 0;
    }

private:
    bool succeed_ = false;
};

}
#endif // DOWNLOADERGETFILEINFOHANDLER_H
