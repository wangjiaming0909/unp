#include <atomic>
#include <boost/system/error_code.hpp>
#include <memory>

#include "reactor/epoll_reactor_impl.h"
#include "reactor/reactor_implementation.h"
#include "sync_client.h"
#include "net/inet_addr.h"
#include "sync_handler.h"
#include "reactor/connector.h"

namespace filesync
{

SyncClient::SyncClient(const net::inet_addr& serverAddr) 
    : serverAddr_(serverAddr)
    , syncPath_{}
    , reactor_{nullptr}
    , timer_{nullptr}
    , manager_{nullptr}
{
    using namespace reactor;
    using namespace std::chrono_literals;
    reactor_ = new Reactor(new epoll_reactor_impl(), true);
    manager_.reset(new ConnectionManager(*reactor_));
    timer_.reset(new HHWheelTimer(&*reactor_));
    timeoutHandler_.reset(new TimeoutHandler(*reactor_));
    //timer_->scheduleTimeout(*timeoutHandler_, 1s);
}

SyncClient::~SyncClient()
{
    delete reactor_;
}

int SyncClient::start(const std::atomic_int& cancelToken)
{
    if(!cancelToken.load())
    {
        return -2;
    }
    connect();
    monitorLocalFolder();

    reactor_->start();
    int ret = 0;
    while(ret >= 0)
    {
        ret = reactor_->handle_events();
    }
    return ret;
}

bool SyncClient::setMonitorFolder(const std::string& fullPath)
{
    syncPath_.assign(fullPath.begin(), fullPath.end());
    if(!boost::filesystem::is_directory(syncPath_))
    {
        LOG(ERROR) << "path is not a directory: " << fullPath;
        return false;
    }
    if(!boost::filesystem::exists(syncPath_))
    {
        boost::system::error_code err_code;
        if(!boost::filesystem::create_directory(syncPath_, err_code))
        {
            LOG(WARNING) << "create directory failed: " << err_code.message();
            return false;
        }
    }
    return true;
}

void SyncClient::monitorLocalFolder()
{

}

int SyncClient::connect()
{
    using namespace std::chrono_literals;
    auto* connection = manager_->makeConnection<reactor::connector<ServerMonitorHandler>>();
    serverMonitorHandler_ = connection->connect(serverAddr_, 1s);
    if (!serverMonitorHandler_) {
        serverStatus_ = ServerStatus::disconnected;
        return -1;
    } 

    serverStatus_ = ServerStatus::connected;
    return 0;
}

}
