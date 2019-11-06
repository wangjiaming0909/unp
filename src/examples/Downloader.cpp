#include "Downloader.h"
#include "util/easylogging++.h"
#include "reactor/connector.h"
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "examples/downloadergetfileinfohandler.h"

namespace examples
{

Downloader::Downloader(const string_t& url)
    : targetAddr_{}
    , url_{url}
    , fileName_{}
    , fileSize_{0}
    , targetPath_{}
    , urlParser_{}
    , clientPtr_{nullptr}
{
    urlParser_.init(url_);
    if(!urlParser_.valid())
    {
        url_ = "";
        LOG(WARNING) << "url format error...";
    }
    initTcpClient();
    retriveAddrFromUrl();
}

void Downloader::retriveAddrFromUrl()
{
    if(urlParser_.scheme() == "https")
    {
        isSSL_ = true;
    }else if(urlParser_.scheme() == "http") {
        isSSL_ = false;
    }else{
        LOG(WARNING) << "unsuported scheme...";
        return;
    }

    addrinfo* addrs = nullptr;

    char host[64] = {};
    memcpy(host, urlParser_.host().cbegin(), urlParser_.host().size());

    int ret = ::getaddrinfo(host, nullptr, nullptr, &addrs);

    if(addrs == nullptr)
    {
        LOG(ERROR) << "didn't get addrinfo: " << strerror(errno);
    }

    auto *ap = &addrs[0];

    if(ap)
    {
        sockaddr_in* inaddr = (sockaddr_in*)(ap->ai_addr);
        targetAddr_ = *inaddr;
        // ap = ap->ai_next;
    }

    freeaddrinfo(addrs);
    if(isSSL_) targetAddr_.set_port_number(443);
    else targetAddr_.set_port_number(80);
}

void Downloader::initTcpClient()
{
    clientPtr_.reset(new reactor::tcp_client{unp::reactor_imp_t_enum::USING_EPOLL});
}

int Downloader::download()
{

    auto ret = getFileInfo();
    if(ret != 0) return ret;

    using namespace std::chrono_literals;
    auto callback = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_NONE, "");
    auto* connector = clientPtr_->addConnection<Connector_t>(std::move(callback), isSSL_);
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return -1;
    }
    connection->initFileWriter(fileName_.c_str());
    ret = clientPtr_->start();
    return ret;
}

int Downloader::requestSetupCallback(http::HttpMessage &mes, http::HttpHeaderCode code, const string_t& extraHeaderValue)
{
    using namespace http;
    mes.setHttpVersion(1, 1);
    mes.setRequestPath(urlParser_.path().cbegin());
    mes.setRequestMethod(HTTPMethod::GET);
    mes.addHeader(HttpHeaderCode::HTTP_HEADER_HOST, urlParser_.host());
    mes.addHeader(HttpHeaderCode::HTTP_HEADER_USER_AGENT, USERAGENT);
    mes.addHeader(HttpHeaderCode::HTTP_HEADER_CONNECTION, CONNECTION);
    mes.addHeader(HttpHeaderCode::HTTP_HEADER_ACCEPT, ACCEPT);
    mes.addHeader(HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING, ACCEPTENCODING);
    mes.addHeader(HttpHeaderCode::HTTP_HEADER_ACCEPT_LANGUAGE, ACCEPTLANGUAGE);
    if(code != HttpHeaderCode::HTTP_HEADER_NONE)
        mes.addHeader(code, extraHeaderValue);
    return 0;
}

int Downloader::getFileInfo()
{
    using namespace std::chrono_literals;
    auto callback = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_NONE, "");
    auto* connector = clientPtr_->addConnection<reactor::connector<DownloaderGetFileInfoHandler>>(std::move(callback), isSSL_);
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return -1;
    }
    auto ret = clientPtr_->start();

    auto status = connection->getStatus();
    const std::string* cd = nullptr;
    const std::string* location = nullptr;
    switch(status)
    {
    case 302:
        LOG(INFO) << "connector get a 302";
        location = connection->getResponseHeaderValue(http::HttpHeaderCode::HTTP_HEADER_LOCATION);
        if(location == nullptr)
        {
            LOG(ERROR) << "server returned 302, but no location header...";
            ret = -1;
            break;
        }
        url_ = *location;
        urlParser_ = http::URLParser(url_);
        retriveAddrFromUrl();
        return getFileInfo();
        break;
    case 200:
    case 206:
        LOG(INFO) << "connector get a " << status;
        fileSize_ = connection->getContentLength();
        cd = connection->getFileName();
        if(cd == nullptr)
        {
            LOG(WARNING) << "get fileName error...";
            fileName_ = "DEFAULTFILENAME.unp";
            ret = -1;
        }
        else
        {
            fileName_ = std::string{cd->begin() + strlen("attachment; filename=\""), cd->end() - 1};
            ret = 0;
        }
        break;
    default:
        LOG(ERROR) << "get file info error: " << status;
        ret = -1;
        break;
    }

    clientPtr_->closeConnection<reactor::connector<DownloaderGetFileInfoHandler>>(*connector, 1s);
    return ret;
}

Downloader::~Downloader()
{
}
}
