#include <atomic>
#include <boost/filesystem/operations.hpp>
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
    reactor_.reset(new Reactor(new epoll_reactor_impl(), true));
    manager_.reset(new ConnectionManager(*reactor_));
    timer_.reset(new HHWheelTimer(&*reactor_));
}

SyncClient::~SyncClient(){}

int SyncClient::start(const std::atomic_int& cancelToken)
{

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

void SyncClient::monitor()
{
    
}

int SyncClient::connect()
{
    manager_->makeConnection<reactor::connector<SyncHandler>>();
}

}
