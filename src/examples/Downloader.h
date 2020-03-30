#pragma once
#include <string>
#if __cplusplus > 20130101
#include <filesystem>
#endif
#include <memory>

#include "boost/noncopyable.hpp"
#include "util/FileWriter.h"
#include "reactor/tcp_client.h"
#include "http/http_parser/URLParser.h"
#include "net/inet_addr.h"
#include "examples/DownloaderHandler.h"
#include "net/unp.h"

namespace examples
{

class Downloader : boost::noncopyable
{
public:
    using string_t = std::string;
    using Connector_t = reactor::connector<examples::DownloaderHandler>;
    Downloader(const string_t& url);
    ~Downloader();

public:
    void setDownloadPath(const string_t &/*path*/)
    {
    }

    void setConnectorNum(uint8_t connectNum) { connectNum_ = connectNum; }
    void setThreadNum(uint8_t threadNum) { threadNum_ = threadNum; }
    int getFileInfo();
    int download();

TEST_PRIVATE:
    void initTcpClient();
    int chunkDownload();
    void retriveAddrFromUrl();
    int requestSetupCallback(http::HttpMessage &mes, http::HttpHeaderCode headerCode, const string_t &extraHeaderValue);
    std::vector<std::string> divideRanges(uint8_t n);
    std::vector<Connector_t *> rangeDownload(uint8_t n, const std::vector<std::string> &ranges);
    void retriveFileNameFromContentDisposition(const std::string &cd);

TEST_PRIVATE: 
    static const char* DEFAULT_DOWNLOAD_PATH;
    static const std::string DEFAULT_FILE_NAME;
    bool isSSL_ = false;
    uint8_t connectNum_ = 1;
    uint8_t threadNum_ = 1;
    net::inet_addr targetAddr_;
    string_t url_;
    string_t fileName_;
    uint64_t fileSize_;
#if __cplusplus > 20130101
    std::filesystem::path targetPath_;
#else 
    std::string targetPath_;
#endif
    http::URLParser urlParser_;
    std::shared_ptr<reactor::tcp_client> clientPtr_;
    bool isChunked_ = false;
};

static string_piece::const_string_piece USERAGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:71.0) Gecko/20100101 Firefox/71.0";
static string_piece::const_string_piece ACCEPT = "*/*";
// static string_piece::const_string_piece ACCEPTENCODING = "gzip, deflate, br";
static string_piece::const_string_piece ACCEPTENCODING = "identity";
static string_piece::const_string_piece ACCEPTLANGUAGE = "en-US,en;q=0.5";
static string_piece::const_string_piece CONNECTION = "keep-alive";

}
