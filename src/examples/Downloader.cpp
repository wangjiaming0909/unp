#include "Downloader.h"
#include "util/easylogging++.h"
#include "reactor/connector.h"
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>

namespace examples
{
const std::string Downloader::DEFAULT_FILE_NAME = "unp.unp";

const char* Downloader::DEFAULT_DOWNLOAD_PATH = "/tmp";

Downloader::Downloader(const string_t& url)
    : targetAddr_{}
    , url_{url}
    , fileName_{}
    , fileSize_{0}
    , targetPath_{}
    , urlParser_{}
    , clientPtr_{nullptr}
{
    urlParser_.init(url_);
    if(!urlParser_.valid())
    {
        url_ = "";
        LOG(WARNING) << "url format error...";
    }
    initTcpClient();
    retriveAddrFromUrl();
}

Downloader::~Downloader()
{
}

void Downloader::retriveAddrFromUrl()
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

void Downloader::initTcpClient()
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

std::vector<std::string> Downloader::divideRanges(uint8_t n)
{
  std::vector<std::string> ranges{};
  if(fileSize_ <= 0) return ranges;
  ranges.resize(n);
  uint64_t start = 0;
  uint64_t stop = -1;
  for(uint8_t i = 0; i < n - 1; i++)
  {
    start = stop + 1;
    stop = start + fileSize_ / n;
    ranges[i].append("bytes=").append(std::to_string(start)).append("-").append(std::to_string(stop));
  }
  start = stop + 1;
  ranges[n - 1].append("bytes=").append(std::to_string(start)).append("-").append(std::to_string(fileSize_));
  return ranges;
}

std::vector<Downloader::Connector_t*> Downloader::rangeDownload(uint8_t n, const std::vector<std::string>& ranges)
{
  std::vector<Connector_t*> ret{};
  if(n != ranges.size()) return ret;
  ret.resize(n);
  for(uint8_t i = 0; i < n; i++)
  {
    auto callback = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_RANGE, ranges[i]);
    auto* connector = clientPtr_->addConnection<Connector_t>(std::move(callback), isSSL_);
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
      LOG(ERROR) << "connect failed...";
      return ret;
    }
    //connection->setDownloadRange(ranges
    connection->setWhenToCloseConnection(http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
    connection->initWriter((fileName_.append(std::to_string(i))).c_str());
    ret[i] = connector;
  }
  return ret;
}

int Downloader::chunkDownload()
{
  auto callback = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_NONE, "");
  auto* connector = clientPtr_->addConnection<Connector_t>(std::move(callback), isSSL_);
  auto* connection = connector->connect(targetAddr_, 2s);
  if(connection == nullptr)
  {
    LOG(ERROR)	 << "connect failed...";
    return -1;
  }
  connection->setWhenToCloseConnection(http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
  connection->initWriter(fileName_.c_str());
  clientPtr_->start();
  return 0;
}

int Downloader::download()
{
    if(!urlParser_.valid()) return -1;
    using namespace std::chrono_literals;
    
    if(isChunked_) return chunkDownload();

    uint8_t n = 2;
    auto ranges = divideRanges(n);
    if(ranges.size() == 0)
    {
        return -1;
    } 
	auto connectors = rangeDownload(n, ranges);
	clientPtr_->start();

	for(uint8_t i = 0; i < n; i++)
	{
		if(connectors[i] != nullptr)
		{
			clientPtr_->closeConnection<Connector_t>(*connectors[i], 1s);
		}
	}
	return 0;
}

/*
int Downloader::download()
{
    if(!urlParser_.valid()) return -1;
    using namespace std::chrono_literals;

    int r = 0;

    std::string range1 = "bytes=0-";
    range1.append(std::to_string(fileSize_/2));

    std::string range2 = "bytes=";
    range2.append(std::to_string(fileSize_/2 + 1)).append("-").append(std::to_string(fileSize_));

    auto callback1 = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_RANGE, range1);
    auto callback2 = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_RANGE, range2);
    auto* connector1 = clientPtr_->addConnection<Connector_t>(std::move(callback1), isSSL_);
    auto* connector2 = clientPtr_->addConnection<Connector_t>(std::move(callback2), isSSL_);

    auto* connection1 = connector1->connect(targetAddr_, 2s);
    auto* connection2 = connector2->connect(targetAddr_, 2s);
    if(connection1 == nullptr || connection2 == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return -1;
    }
    connection1->setWhenToCloseConnection(http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
    connection2->setWhenToCloseConnection(http::Http1xCodec::CodecState::ON_MESSAGE_COMPLETE);
    connection1->initWriter((fileName_ + "1").c_str());
    connection2->initWriter((fileName_ + "2").c_str());
    auto ret = clientPtr_->start();
    clientPtr_->closeConnection<Connector_t>(*connector1, 2s);
    clientPtr_->closeConnection<Connector_t>(*connector2, 2s);
    return ret;
}
*/

int Downloader::requestSetupCallback(http::HttpMessage &mes, http::HttpHeaderCode code, const string_t& extraHeaderValue)
{
    mes.setHttpVersion(1, 1);
    mes.setRequestPath(urlParser_.path().cbegin());
    mes.setRequestMethod(http::HTTPMethod::GET);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_HOST, urlParser_.host());
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_USER_AGENT, USERAGENT);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT, ACCEPT);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING, ACCEPTENCODING);
    // mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_CONNECTION, CONNECTION);
    if(code != http::HttpHeaderCode::HTTP_HEADER_NONE) mes.addHeader(code, extraHeaderValue);
    return 0;
}

int Downloader::getFileInfo()
{
    if(!urlParser_.valid()) return -1;
    using namespace std::chrono_literals;

    int r = 0;

    auto callback = std::bind(&Downloader::requestSetupCallback, this, std::placeholders::_1, http::HttpHeaderCode::HTTP_HEADER_NONE, "");
    auto* connector = clientPtr_->addConnection<Connector_t>(std::move(callback), isSSL_);
    auto* connection = connector->connect(targetAddr_, 2s);
    if(connection == nullptr)
    {
        LOG(ERROR) << "connect failed...";
        return -1;
    }
    connection->setWhenToCloseConnection(http::Http1xCodec::CodecState::ON_HEADERS_COMPLETE);
    clientPtr_->start();

    auto status = connection->codec_.status();
    auto& message = connection->codec_.message();

    if(status != 200) 
    {
        LOG(ERROR) << "Connect returned: " << connection->codec_.status();
    }

    if(status == 302)
    {
        if(message.hasHeader(http::HttpHeaderCode::HTTP_HEADER_CONTENT_LENGTH))
        {
            fileSize_ = connection->codec_.contentLength();
        }
        // else if(connection->codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_TRANSFER_ENCODING)))
        auto *location = message.getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_LOCATION);
        if(location == nullptr)
        {
            LOG(WARNING) << "server returned 302, but no LOCATION header...";
            clientPtr_->closeConnection<Connector_t>(*connector, 1s);
            return -1;
        }
        url_ = *location;
        urlParser_.reset(url_);
        retriveAddrFromUrl();
        return getFileInfo();
    }

    if(status == 200)
    {
        if(message.hasHeader(http::HttpHeaderCode::HTTP_HEADER_CONTENT_LENGTH))
        {
            fileSize_ = connection->codec_.contentLength();
        }
        if(message.hasHeader(http::HttpHeaderCode::HTTP_HEADER_TRANSFER_ENCODING))
        {
            if(*message.getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_TRANSFER_ENCODING) == "chunked")
            {
                LOG(WARNING) << "should not be chuncked...";
                isChunked_ = true;
            }
        }
        auto* cd = message.getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_CONTENT_DISPOSITION);
        if(cd == nullptr) 
        {
            LOG(WARNING) << "get fileName error...";
            fileName_ = DEFAULT_FILE_NAME;
        }
        else
        {
            retriveFileNameFromContentDisposition(*cd);
            LOG(INFO) << "fileName: " << fileName_;
            LOG(INFO) << "fileSize: " << fileSize_;
        }
        r = 0;
    }else
    {
        r = -1;
    }

    clientPtr_->closeConnection<Connector_t>(*connector, 1s);
    return r;
}

void Downloader::retriveFileNameFromContentDisposition(const std::string& cd)
{
    // fileName_ = std::string{cd.begin() + strlen("attachment; filename="), cd.end() - 1};
    auto it = std::find(cd.begin(), cd.end(), '=');
    if(it == cd.end())
    {
        LOG(WARNING) << "Unknown content disposition...";
        fileName_ = DEFAULT_FILE_NAME;
    }
    std::string::const_iterator begin = ++it;
    std::string::const_iterator end = cd.end();
    if (*(it) == '"' || *(it) == '\'')
    {
        begin = ++it;
    }
    if(cd.back() == '"' || cd.back() == '\'')
    {
        end--;
    }
    fileName_ = std::string{begin, end};
}
}
