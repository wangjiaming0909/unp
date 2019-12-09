#pragma once
#include "reactor/connection_handler.h"
#include "http/Http1xCodec.h"
#include "http/http_parser/URLParser.h"
#include "util/FileWriter.h"
#include <memory>

namespace examples
{

class Downloader;

class DownloaderHandler : public reactor::connection_handler, http::HttpCodec::Callback{
public:
    friend class Downloader;
    using MessageSetupCallback_t = std::function<int(http::HttpMessage &mes)>;
    DownloaderHandler(reactor::Reactor &react, MessageSetupCallback_t&& callback, bool isSSL)
        : connection_handler{react, isSSL}
        , request_{}
        , codec_{http::HttpDirection::DOWNSTREAM}
        , mesSetupCallback_{callback}
    {
        codec_.setCallback(this);
    }

    ~DownloaderHandler()
    {
        LOG(INFO) << "Closing a DownloaderHandler";
        if(fileWriterPtr_)
        {
            if(fileWriterPtr_->isValid())
                fileWriterPtr_->close();
        }
    }

    virtual int handle_input(int handle) override;

    virtual int open() override;

    void initWriter(const char* fileName)
    {
        fileWriterPtr_.reset(new utils::FileWriter{fileName});
    }

    void setWhenToCloseConnection(http::Http1xCodec::CodecState state)
    {
        whenToClose_ = state;
    }

    virtual int onStatus(const char* buf, size_t len) override;
    virtual int onBody(const char* buf, size_t size) override;
    virtual int onHeadersComplete(size_t len) override;
    virtual int onMessageComplete() override;
    int onChunkHeader(size_t len) override;
    int onChunkComplete() override;

private:
    http::Http1xCodec::CodecState whenToClose_ = http::Http1xCodec::CodecState::IDLE;
    bool isShouldClose_ = false;
    http::HttpMessage request_;
    http::Http1xCodec codec_;
    MessageSetupCallback_t mesSetupCallback_;
    std::shared_ptr<utils::FileWriter> fileWriterPtr_;
    bool isChunked_ = false;
    uint64_t fileSize_ = 0;
};
}
