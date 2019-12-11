#include "downloader/download.h"
#include "http/gperf/HttpHeaderCode.h"
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <utility>
#include <vector>

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

Download::~Download(){}

int Download::download()
{
	using namespace std::chrono_literals;
	auto pair = download_imp(currentBegin_, currentEnd_);
	clientPtr_->start();
	auto* connection = pair.second;
	if(connection->status_ == Handler::HandlerStatus::NEW_LOCATION_GOT)
	{
		auto& newUrl = connection->url_;
		LOG(INFO) << "302... redirecting to: " << newUrl;
		url_ = newUrl;
		urlParser_.reset(url_);
		retriveAddrFromUrl();
		download();
		goto out;
    }
    if (connection->status_ == Handler::HandlerStatus::RANGE_MATCH)
    {
		auto bytesShouldDownloaded = connection->rangeEnd_ - connection->rangeBegin_ + 1;
		auto totalSize = connection->fileSize_;
		if(totalSize == bytesShouldDownloaded) // completed
		{
			goto out;
		}
		if(connection->bytesDownloaded_ == bytesShouldDownloaded)
		{
			LOG(INFO) << "range from: " << connection->rangeBegin_ << " to: " << connection->rangeEnd_ << " downloaded...";
			auto bytesRemain = totalSize - connection->bytesDownloaded_;
			LOG(INFO) << "remained: " << bytesRemain;
			if(downloadRemain(bytesRemain, connection->rangeEnd_ + 1) < 0)
			{
				LOG(INFO) << "download remain error...";
			}
			goto out;
		}
		else if(retryTimes_ >= 0)
		{
			retryTimes_--;
			download();
			retryTimes_++;
			goto out;
		}
    }
	if(connection->status_ == Handler::HandlerStatus::NOT_RESPONDING_TO_RANGE)
	{

	}
	
out:
	clientPtr_->closeConnection<Connector_t>(*pair.first, 2s);
    return 0;
}

int Download::downloadRemain(uint64_t remain, uint64_t start)
{
	using namespace std::chrono_literals;
	if(remain == 0)
	{
		return 0;
	}
	if(remain < connectNum_)
	{
		auto pair = download_imp(start, start + remain - 1);
		clientPtr_->start();
		auto* connection = pair.second;
		if(connection->status_ == Handler::HandlerStatus::NOT_RESPONDING_TO_RANGE)
		{
			LOG(INFO) << "download error should respond to range header....";
		}
		clientPtr_->closeConnection<Connector_t>(*pair.first, 2s);
		return 0;
	}
	std::vector<Connector_t*> connectors{};
	std::vector<Handler*> connections{};
	for(int i = 0; i < connectNum_; i++)
	{
		auto pair = download_imp(start, start + remain / connectNum_ - 1);
		start = start + remain / connectNum_;
		remain -= remain / connectNum_;
		connectors.push_back(pair.first);
		connections.push_back(pair.second);
	}
	if(remain > 0)
	{
		auto pair = download_imp(start, start + remain-1);
		connectors.push_back(pair.first);
		connections.push_back(pair.second);
	}
	clientPtr_->start();

	for(auto* connector : connectors)
	{
		clientPtr_->closeConnection<Connector_t>(*connector, 2s);
	}
	return 0;
}

void Download::HandlerSetupCallback(Handler& handler, uint64_t begin, uint64_t end)
{
	handler.setDownloadRange(begin, end);
}

std::pair<Download::Connector_t*, Handler*>Download::download_imp(uint64_t begin, uint64_t end)
{
	auto callback = std::bind(&Download::HandlerSetupCallback, this, std::placeholders::_1, begin, end);
    auto* connector = clientPtr_->addConnection<Connector_t>(url_, isSSL_, std::move(callback));
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return std::make_pair(nullptr, nullptr);
    }
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
