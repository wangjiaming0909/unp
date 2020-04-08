#pragma once
#include <string>
#include <memory>
#include <atomic>

#include <boost/filesystem.hpp>
#include "reactor/reactor.h"
#include "net/inet_addr.h"
#include "reactor/HHWheelTimer.h"
#include "reactor/ConnectionManager.h"
#include "reactor/TimeoutHandler.h"

namespace filesync {
class SyncClient
{
public:
    SyncClient(const net::inet_addr& serverAddr);
    ~SyncClient();
    int start(const std::atomic_int& cancelToken);

    bool setMonitorFolder(const std::string& fullPath);


private:
    //monitor dir
    void monitor();
    //connect to server
    int connect();

private:
    net::inet_addr serverAddr_;
    boost::filesystem::path syncPath_;
    reactor::Reactor* reactor_;
    std::unique_ptr<reactor::HHWheelTimer> timer_;
    std::unique_ptr<reactor::ConnectionManager> manager_;
    std::unique_ptr<reactor::TimeoutHandler> timeoutHandler_;
};

}
