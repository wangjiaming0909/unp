#pragma once
#include <string>
#include <filesystem>
#include <memory>

#include "boost/noncopyable.hpp"
#include "util/FileWriter.h"
#include "reactor/tcp_client.h"
#include "http/http_parser/URLParser.h"
#include "net/inet_addr.h"
#include "examples/DownloaderHandler.h"

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
    void setDownloadPath(const string_t &path)
    {
    }

    void setConnectorNum(uint8_t connectNum) { connectNum_ = connectNum; }
    void setThreadNum(uint8_t threadNum) { threadNum_ = threadNum; }
    int getFileInfo();
    int download();

private:
    void initTcpClient();
    void retriveAddrFromUrl();
    int requestSetupCallback(http::HttpMessage &mes, http::HttpHeaderCode headerCode, const string_t& extraHeaderValue);

private:
    static constexpr char *DEFAULT_DOWNLOAD_PATH = "/tmp";
    bool isSSL_ = false;
    uint8_t connectNum_ = 1;
    uint8_t threadNum_ = 1;
    net::inet_addr targetAddr_;
    string_t url_;
    string_t fileName_;
    string_t fileSize_;
    std::filesystem::path targetPath_;
    http::URLParser urlParser_;
    std::shared_ptr<reactor::tcp_client> clientPtr_;
};
}