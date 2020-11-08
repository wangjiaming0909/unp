#include "downloader/download.h"
#include "http/gperf/HttpHeaderCode.h"
#include "net/inet_addr.h"
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <utility>
#include <vector>

namespace downloader
{

Download::Download(int id, const std::string& url, std::shared_ptr<DownloadStateCallback> callback)
  : id_(id)
  , url_{url}
  , callback_(callback)
  , failedRanges_{}
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

int Download::downloadEX()
{
  using namespace std::chrono_literals;
  if (url_ == "") return -1;
  int succeed = 1;
  auto pair = connect(currentBegin_, currentEnd_);
  clientPtr_->start();
  auto* connection = pair.second;
  if(connection == nullptr) return -1;
  size_ = bytesRemained_= connection->fileSize_;
  if(connection->status_ == Handler::HandlerStatus::NEW_LOCATION_GOT) {
    auto& newUrl = connection->url_;
    LOG(INFO) << "302... redirecting to: " << newUrl;
    url_ = newUrl;
    urlParser_.reset(url_);
    retriveAddrFromUrl();
    succeed = downloadEX();
    goto out;
  }
  if (connection->status_ == Handler::HandlerStatus::RANGE_MATCH) {
    auto bytesShouldDownloaded = connection->rangeEnd_ - connection->rangeBegin_ + 1;
    auto totalSize = connection->fileSize_;
    if(totalSize == bytesShouldDownloaded) // completed
    {
      succeed = 0;
      goto out;
    }
    if(connection->bytesDownloaded_ == bytesShouldDownloaded) {
      LOG(INFO) << "range from: " << connection->rangeBegin_ << " to: " << connection->rangeEnd_ << " downloaded...";
      auto bytesRemain = totalSize - connection->bytesDownloaded_;
      LOG(INFO) << "remained: " << bytesRemain;
      succeed = 0;
      if(downloadRemain(bytesRemain, connection->rangeEnd_ + 1) < 0)
      {
        LOG(INFO) << "download remain error...";
        succeed = -1;
      }
      goto out;
    } else if(retryTimes_ >= 0) {
      retryTimes_--;
      succeed = downloadEX();
      retryTimes_++;
      goto out;
    }
  }
  if(connection->status_ == Handler::HandlerStatus::NOT_RESPONDING_TO_RANGE) {
    LOG(ERROR) << "NOT_RESPONDING_TO_RANGE..";
    succeed = 1;
  } else {
    succeed = -1;
  }

out:
  clientPtr_->closeConnection<Connector_t>(*pair.first, 10ms);
  return succeed;
}

int Download::download()
{
  auto succeed = downloadEX();
  if (succeed >= 0 && callback_ && bytesRemained_ == 1001)
  {
    callback_->taskCompleted(id_);
  }
  else if(callback_)
  {
    callback_->taskFailed(id_, "connect failed");
    return -1;
  }
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
    auto pair = connect(start, start + remain - 1);
    clientPtr_->start();
    auto* connection = pair.second;
    if(connection->status_ == Handler::HandlerStatus::NOT_RESPONDING_TO_RANGE)
    {
      LOG(INFO) << "download error should respond to range header....";
    }
    clientPtr_->closeConnection<Connector_t>(*pair.first, 10ms);
    return 0;
  }
  std::vector<Connector_t*> connectors{};
  std::vector<Handler*> connections{};
  for(int i = 0; i < connectNum_; i++)
  {
    auto pair = connect(start, start + remain / connectNum_ - 1);
    if(pair.first == nullptr || pair.second == nullptr)
    {
      failedRanges_.push_back(std::make_pair(start, start + remain - 1));
      continue;
    }
    start = start + remain / connectNum_;
    remain -= remain / connectNum_;
    connectors.push_back(pair.first);
    connections.push_back(pair.second);
  }
  if(remain > 0)
  {
    auto pair = connect(start, start + remain-1);
    if(pair.first == nullptr || pair.second == nullptr)
    {
      failedRanges_.push_back(std::make_pair(start, start + remain - 1));
    }else
    {
      connectors.push_back(pair.first);
      connections.push_back(pair.second);
    }
  }
  clientPtr_->start();

  for(auto* connector : connectors)
  {
    clientPtr_->closeConnection<Connector_t>(*connector, 10ms);
  }
  return 0;
}

void Download::HandlerSetupCallback(Handler& handler, uint64_t begin, uint64_t end)
{
  handler.setDownloadRange(begin, end);
}

std::pair<Download::Connector_t*, Handler*>Download::connect(uint64_t begin, uint64_t end)
{
  //auto callback = std::bind(&Download::HandlerSetupCallback, this, std::placeholders::_1, begin, end);
  auto* connector = clientPtr_->addConnection<Connector_t>(url_, isSSL_, begin, end, shared_from_this());
  auto* connection = connector->connect(targetAddr_, 100ms);
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

void Download::downloadUpdateCallback(uint64_t , uint64_t , uint64_t bytesDone)
{
  bytesRemained_ -= bytesDone;
  float percent = 0.0;
  if(size_ == 0)
  {
    //LOG(WARNING) << "No size info...";
    percent = 0;
  }else
  {
    percent = (size_ - bytesRemained_) / (float)size_;
  }
  callback_->taskUpdated(id_, percent);
}

}
