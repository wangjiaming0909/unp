#include "Downloader.h"
#include "util/easylogging++.h"
#include "reactor/connector.h"
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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
    urlParser_.init(url);
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
    if(urlParser_.scheme() == "https")
    {
        isSSL_ = true;
    }else if(urlParser_.scheme() == "http") {
        isSSL_ = false;
    }else{
        LOG(WARNING) << "unsuported scheme...";
        return;
    }
    clientPtr_.reset(new reactor::tcp_client{unp::reactor_imp_t_enum::USING_EPOLL});
}

int Downloader::download()
{

}

int Downloader::requestSetupCallback(http::HttpMessage &mes, http::HttpHeaderCode code, const string_t& extraHeaderValue)
{
    return 0;
}

int Downloader::getFileInfo()
{
    using namespace std::chrono_literals;
    auto callback = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_RANGE, "bytes=1-1");
    auto* connector = clientPtr_->addConnection<Connector_t>(std::move(callback), isSSL_);
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return -1;
    }
    auto ret = clientPtr_->start();

    if(connection->codec_.status() != 200) 
    {
        LOG(ERROR) << "Connect returned: " << connection->codec_.status();
        return -1;
    }
    fileSize_ = connection->codec_.contentLength();
    auto* cd = connection->codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_CONTENT_DISPOSITION);
    if(cd == nullptr) 
    {
        LOG(WARNING) << "get fileName error...";
        fileName_ = "DEFAULTFILENAME.unp";
    }
    else
    {
        fileName_ = std::string{cd->begin() + strlen("attachment;"), cd->end()};
    }

    clientPtr_->closeConnection<Connector_t>(*connector, 1s);
    return 0;
}

Downloader::~Downloader()
{
}
}