#pragma once
#include "net/inet_addr.h"
#include "reactor/tcp_server.h"
#include <boost/noncopyable.hpp>
#include "downloader/serverhandler.h"
// #include <vector>
// #include <queue>
// #include <map>

namespace downloader
{

/**
 * 1, start tcp_server with handler: DownloaderServerHandler
 * 2, dsHandler receive data, build Mess, give it to DownloaderServer
 * 3, DownloaderServer got the Mess, parse it, start downloading
 * 
 **/

class DownloaderServer : boost::noncopyable
{
friend class DownloaderServerHandler;

public:
    DownloaderServer(net::inet_addr& listenAddr);
    ~DownloaderServer();

    int start();

private:
private:
    net::inet_addr listenAddr_;
    reactor::tcp_server<DownloaderServerHandler> server_;
    // std::vector<int> ids_;
    // std::queue<httpmessage::Mess*> downloadingQueue_;
    // std::queue<httpmessage::Mess*> pausedQueue_;
    // std::queue<httpmessage::Mess*> finishedQueue_;
    // std::map<int, httpmessage::Mess *> partialMesses_;

private:
    // static const int MAX_CONCURRENT_DOWNLOAD = 4;
};

}
