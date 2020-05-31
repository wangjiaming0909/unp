#include <atomic>
#include <boost/system/error_code.hpp>
#include <memory>

#include "reactor/TimeoutHandler.h"
#include "reactor/epoll_reactor_impl.h"
#include "reactor/reactor_implementation.h"
#include "syncclient/sync_client.h"
#include "net/inet_addr.h"
#include "sync_handler.h"

namespace filesync
{

SyncClient::SyncClient(const net::inet_addr& serverAddr)
  : serverAddr_(serverAddr)
  , syncPath_{}
  , reactor_{nullptr}
  , timer_{nullptr}
  , manager_{nullptr}
  , monitor_{nullptr}
  , cancelToken_(false)
{
  using namespace reactor;
  using namespace std::chrono_literals;
  reactor_ = new Reactor(new epoll_reactor_impl(), true);
  manager_.reset(new ConnectionManager(*reactor_));
  timer_.reset(new HHWheelTimer(&*reactor_));
  timeoutHandler_.reset(new TimerHandler(*reactor_, 3s));
}

SyncClient::~SyncClient()
{
  //delete reactor_;
}

int SyncClient::start(const std::atomic_int& cancelToken)
{
  if(!cancelToken.load())
    return -2;
  if (connect() != 0)
    LOG(ERROR) << "connect to server failed...";
  monitorLocalFolder();

  reactor_->start();
  int ret = 0;
  while(ret >= 0)
    ret = reactor_->handle_events();
  using namespace std::chrono_literals;
  if (monitor_) {
    if (fileMonitorConnector_)
      manager_->closeConnection(*fileMonitorConnector_, 1s);
    cancelToken_.store(true);
    monitor_->stopObserve();
  }
  if (timer_) {
    timer_->cancelAll();
    timeoutHandler_.reset();
  }

  return ret;
}

bool SyncClient::setMonitorFolder(const std::string& fullPath)
{
  syncPath_.assign(fullPath.begin(), fullPath.end());
  if(!boost::filesystem::is_directory(syncPath_)) {
    LOG(ERROR) << "path is not a directory: " << fullPath;
    return false;
  }
  if(!boost::filesystem::exists(syncPath_)) {
    boost::system::error_code err_code;
    if(!boost::filesystem::create_directory(syncPath_, err_code)) {
      LOG(WARNING) << "create directory failed: " << err_code.message();
      return false;
    }
  }
  return true;
}

void SyncClient::monitorLocalFolder()
{
  monitor_.reset(new DirObservable(syncPath_));
  fileMonitorConnector_ = manager_->makeConnection<reactor::connector<FileMonitorHandler>>(*monitor_);
  fileMonitorHandler_ = fileMonitorConnector_->connect(serverAddr_, 1s);
  if (!fileMonitorHandler_) {
    LOG(ERROR) << "fileMonitorHandler connect failed..";
    return;
  }
  fileMonitorHandler_->subscribe();
  monitor_->startObserveAsync(cancelToken_);
}

void SyncClient::timeoutCallback(reactor::TimeoutHandler*)
{
  if (!serverMonitorHandler_) return;
  auto hello = "hello\n";
  auto helloPacakgePtr = filesync::getHelloPackage(hello, PackageType::Client);
  auto bytesWritten = sendPackage(helloPacakgePtr);
  if (bytesWritten <= 0) {
    LOG(ERROR) << "client say hello failed...";
    if (sayHelloFailedWaitInterval_ > 100) return;
    timer_->scheduleTimeout(*timeoutHandler_, std::chrono::seconds(sayHelloFailedWaitInterval_*2));
    return;
  }
  sayHelloFailedWaitInterval_ = 1;
}

uint64_t SyncClient::sendPackage(SyncPackagePtr package)
{
  int64_t size = package->ByteSizeLong();
  char* data = static_cast<char*>(::calloc(size, 1));
  package->SerializeToArray(data, size);
  auto bytesWritten = serverMonitorHandler_->write(size, false);
  bytesWritten += serverMonitorHandler_->write(data, size, true);
  free(data);
  return bytesWritten;
}

int SyncClient::connect()
{
  using namespace std::chrono_literals;
  auto* connector = manager_->makeConnection<reactor::connector<ServerMonitorHandler>>();
  serverMonitorHandler_ = connector->connect(serverAddr_, 1s);
  if (!serverMonitorHandler_)
    return -1;
  timeoutHandler_->timeoutCallback = std::bind(&SyncClient::timeoutCallback, this, std::placeholders::_1);
  timer_->scheduleTimeout(*timeoutHandler_, 1s);
  return 0;
}
}
