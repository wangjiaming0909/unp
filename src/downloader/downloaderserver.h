#pragma once
#include "proto/mess.pb.h"
#include "net/inet_addr.h"
#include "reactor/acceptor.h"
#include "reactor/tcp_server.h"
#include "serverhandler.h"
#include <boost/noncopyable.hpp>
#include <vector>
#include <queue>

namespace downloader
{

class DownloaderServer : boost::noncopyable
{
public:
    DownloaderServer(net::inet_addr& listenAddr);
    ~DownloaderServer();

    int start();

private:
private:
    net::inet_addr listenAddr_;
    reactor::tcp_server<ServerHandler> server_;
    std::vector<int> ids_;
    std::queue<httpmessage::Mess*> downloadingQueue_;
    std::queue<httpmessage::Mess*> pausedQueue_;
    std::queue<httpmessage::Mess*> finishedQueue_;
    
private:
    static const int MAX_CONCURRENT_DOWNLOAD = 4;
};

}
