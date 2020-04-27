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
#include "syncclient/sync_handler.h"
#include "syncclient/monitor_handler.h"
#include "syncclient/timer_handler.h"
#include "syncclient/sync_package.h"

namespace filesync {
class SyncClient
{

public:
    using  SyncPackagePtr = std::shared_ptr<SyncPackage>;
    SyncClient(const net::inet_addr& serverAddr);
    ~SyncClient();
    int start(const std::atomic_int& cancelToken);

    bool setMonitorFolder(const std::string& fullPath);


private:
    //monitor dir
    void monitorLocalFolder();
    //connect to server
    int connect();

    void timeoutCallback(reactor::TimeoutHandler*);
    uint64_t sendPackage(SyncPackagePtr package);

private:
    net::inet_addr serverAddr_;
    boost::filesystem::path syncPath_;
    reactor::Reactor* reactor_;
    std::unique_ptr<reactor::HHWheelTimer> timer_;
    std::unique_ptr<reactor::ConnectionManager> manager_;
    std::unique_ptr<TimerHandler> timeoutHandler_;

    ServerMonitorHandler* serverMonitorHandler_ = nullptr;
    FileMonitorHandler* fileMonitorHandler_ = nullptr;

private:
    bool isSyncing = false;
    uint32_t sayHelloFailedWaitInterval_ = 1;
};

}
