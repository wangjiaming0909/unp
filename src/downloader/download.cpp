#include "downloader/download.h"
#include "http/gperf/HttpHeaderCode.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <utility>

namespace downloader
{

Download::Download(const std::string& url) 
    : url_{url}
{
    urlParser_.init(url_);
    if(!urlParser_.valid())
    {
        url_ = "";
        LOG(ERROR) << "url format error...";
        return;
    }
    initTcpClient();
    retriveAddrFromUrl();
}

Download::~Download()
{}

int Download::download()
{
    auto pair = download_imp();
    auto* connection = pair.second;
    if(connection->status_ == Handler::HandlerStatus::NEW_LOCATION_GOT)
    {
        auto& newUrl = connection->url_;
        LOG(INFO) << "302... redirecting to: " << newUrl;
        url_ = newUrl;
        urlParser_.reset(url_);
        retriveAddrFromUrl();
        download();
    }
    if (connection->status_ == Handler::HandlerStatus::RANGE_MATCH)
    {

    }
    //TODO  how to make decisions?
    return 0;
}

std::pair<Download::Connector_t*, Handler*>Download::download_imp()
{
    auto* connector = clientPtr_->addConnection<Connector_t>(url_, isSSL_);
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return std::make_pair(nullptr, nullptr);
    }
    clientPtr_->start();
    return std::make_pair(connector, connection);
}

void Download::initTcpClient()
{
    if(urlParser_.scheme() == "https")
    {
        isSSL_ = true;
    }else if(urlParser_.scheme() == "http") {
        isSSL_ = false;
    }else{
        LOG(WARNING) << "unsuported scheme...";
    }
    clientPtr_.reset(new reactor::tcp_client{unp::reactor_imp_t_enum::USING_EPOLL});
}

void Download::retriveAddrFromUrl()
{
    addrinfo* addrs = nullptr;

    char host[64] = {};
    memcpy(host, urlParser_.host().cbegin(), urlParser_.host().size());

    ::getaddrinfo(host, nullptr, nullptr, &addrs);

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


}
