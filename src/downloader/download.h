#pragma once
#include "boost/noncopyable.hpp"
#include "downloader/Handler.h"
#include "http/http_parser/URLParser.h"
#include "reactor/tcp_client.h"
#include "net/unp.h"
#include "net/inet_addr.h"
#include <memory>

namespace downloader
{

class Download : boost::noncopyable
{
public:
    using Connector_t = reactor::connector<Handler>;

    Download(const std::string& url);
    virtual ~Download();

    int download();
    
    
TEST_PRIVATE:
    void initTcpClient();
    void retriveAddrFromUrl();
    std::pair<Connector_t*, Handler*> download_imp(uint64_t begin, uint64_t end);
	int downloadRemain(uint64_t size, uint64_t start);
	void HandlerSetupCallback(Handler& handler, uint64_t begin, uint64_t end);

TEST_PRIVATE:
    bool isSSL_ = false;
    uint8_t connectNum_ = 4;
    net::inet_addr targetAddr_;
    std::string url_;
    http::URLParser urlParser_;
    std::shared_ptr<reactor::tcp_client> clientPtr_;
	uint64_t currentBegin_ = 0;
	uint64_t currentEnd_ = 1000;
	uint64_t size_ = 0;

	int retryTimes_ = 3;
};
}
