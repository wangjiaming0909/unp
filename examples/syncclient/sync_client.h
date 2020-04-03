#pragma once
#include <string>
#include <memory>
#include <atomic>

#include "boost/filesystem.hpp"
#include "reactor/reactor.h"
#include "net/inet_addr.h"
#include "reactor/HHWheelTimer.h"
#include "reactor/ConnectionManager.h"

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
    std::shared_ptr<reactor::Reactor> reactor_;
    std::shared_ptr<reactor::HHWheelTimer> timer_;
    std::shared_ptr<reactor::ConnectionManager> manager_;
};

}
