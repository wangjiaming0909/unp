#pragma once
#include "reactor/sock_connection_handler.h"
#include "http/Http1xCodec.h"
#include "http/HttpMessage.h"
#include "http/http_parser/URLParser.h"
#include "util/FileWriter.h"

#include <functional>
#include <memory>

namespace downloader
{

class Download;
class Handler : public reactor::sock_connection_handler, public http::HttpCodec::Callback
{
public:
    friend class Download;
    enum class HandlerStatus
    {
        IDLE = 0,
        REQUEST_SENT,
        RECEIVED302,
        NEW_LOCATION_GOT,
        RECV302_WITHOUT_NEW_LOCATION,
        RECEIVED200,
        CHUNK_ENCODING,
        NOT_RESPONDING_TO_RANGE,
        RANGE_NOT_MATCH,
        RANGE_MATCH,
        PARTIALLY_DATA_GOT,
        NO_CONTENT_DISPOSITION
    };

public:
    using MessageSetupCallback_t = std::function<void(Handler& handler)>;
    Handler(reactor::Reactor& react, const std::string& url, bool isSSL, uint64_t begin, uint64_t end, std::shared_ptr<Download> downloader);
    virtual ~Handler();

    virtual int handle_input(int handle) override;
    virtual int open() override;

    virtual int onStatus(const char* buf, size_t len) override;
    virtual int onBody(const char* buf, size_t size) override;
    virtual int onHeadersComplete(size_t len) override;
    virtual int onMessageComplete() override;
    virtual int onHeaderField(const char *buf, size_t len) override;
    virtual int onHeaderValue(const char *buf, size_t len) override;
    int onChunkHeader(size_t len) override;
    int onChunkComplete() override;

    void setDownloadRange(uint64_t begin, uint64_t end)
    {
       usingRangeDownload_ = true;
       rangeBegin_ = begin;
       rangeEnd_ = end;
    }
    uint64_t bytesRemainToDownload() const
    {
        if(!usingRangeDownload_) return 0;
        return rangeEnd_ - rangeBegin_ - bytesDownloaded_ + 1;
    }

    void initFileWriter()
    {
        fileWriterPtr_.reset(new utils::FileWriter{fileName_.c_str()});
    }


private:
	std::pair<uint64_t, uint64_t> parseContentRangeHeader(const std::string& headerValue);
	void retriveFileNameFromContentDisposition(const std::string& cd);

private:
    uint64_t fileSize_ = 0;
    uint64_t rangeBegin_ = 0;
    uint64_t rangeEnd_ = 0;
    uint64_t bytesDownloaded_ = 0;

    bool isShouldClose_ = false;
    bool usingRangeDownload_ = false;

    http::HttpMessage request_;
    http::Http1xCodec codec_;
    http::URLParser urlParser_;
    std::string url_;
    HandlerStatus status_ = HandlerStatus::IDLE;
	std::string fileName_;
    std::shared_ptr<utils::FileWriter> fileWriterPtr_;
	//MessageSetupCallback_t setupCallback_;
    std::shared_ptr<Download> downloader_;
};
static string_piece::const_string_piece USERAGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:71.0) Gecko/20100101 Firefox/71.0";
static string_piece::const_string_piece ACCEPT = "*/*";
// static string_piece::const_string_piece ACCEPTENCODING = "gzip, deflate, br";
static string_piece::const_string_piece ACCEPTENCODING = "identity";
static string_piece::const_string_piece ACCEPTLANGUAGE = "en-US,en;q=0.5";
static const std::string DEFAULT_FILE_NAME = "unp.unp";
// static string_piece::const_string_piece CONNECTION = "keep-alive";

}
