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

    virtual int handle_input(int handle) override
    {
        if(connection_handler::handle_input(handle) < 0)
        {
            LOG(WARNING) << "error when connection_handler::handle_input";
            return -1;
        }

        if(input_buffer_.buffer_length() == 0) 
        {
            LOG(INFO) << "didn't get any data...";
            return 0;
        }

        while(input_buffer_.buffer_length() > 0)
        {
            auto firstChain = input_buffer_.begin().chain();
            auto data = firstChain.get_start_buffer();
            auto chainLen = firstChain.size();
            string_piece::const_string_piece sp{static_cast<const char*>(data), chainLen};
            size_t bytesRead = codec_.onIngress(sp);
            if(codec_.hasError())
            {
                LOG(WARNING) << "Http parser parse error";
                codec_.pause(1);
                return -1;
            }
            assert(bytesRead == chainLen);
            input_buffer_.drain(chainLen);
            LOG(INFO) << "Consumed " << chainLen << " bytes...";
        }
        if(isShouldClose_) return -1;
        return 0;
    }

    virtual int open() override
    {
        int ret = 0;
        if(mesSetupCallback_)
        {
            ret = mesSetupCallback_(request_);
        }
        if(ret != 0) return -1;

        auto messageStr = request_.buildRequestMessage();
        LOG(INFO) << *messageStr;
        auto bytesWritten = write(messageStr->c_str(), messageStr->size());
        if (bytesWritten <= 0) return -1;

        return enable_reading();
    }

    void initWriter(const char* fileName)
    {
        fileWriterPtr_.reset(new utils::FileWriter{fileName});
    }

    virtual int onStatus(const char* buf, size_t len) override
    {
        // if(codec_.status() != 200)
        // {
        //     LOG(WARNING) << "status is: " << codec_.status();
        //     return -1;
        // }
    }
    virtual int onBody(const char* buf, size_t size) override
    {
        if(fileWriterPtr_)
        {
            fileWriterPtr_->write(buf, size);
            fileWriterPtr_->flush();
            return 0;
        }
    }

    virtual int onHeadersComplete(size_t len) override
    {
        if(whenToClose_ == http::Http1xCodec::CodecState::ON_HEADERS_COMPLETE)
        {
            isShouldClose_ = true;
        }
        return 0;
    }

    virtual int onMessageComplete() override
    {
        if(whenToClose_ == http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE)
        {
            isShouldClose_ = true;
        }
        LOG(INFO) << "Message completed...";
        return 0;
    }

    void setWhenToCloseConnection(http::Http1xCodec::CodecState state)
    {
        whenToClose_ = state;
    }

private:
    http::Http1xCodec::CodecState whenToClose_ = http::Http1xCodec::CodecState::IDLE;
    bool isShouldClose_ = false;
    http::HttpMessage request_;
    http::Http1xCodec codec_;
    MessageSetupCallback_t mesSetupCallback_;
    std::shared_ptr<utils::FileWriter> fileWriterPtr_;
};
}