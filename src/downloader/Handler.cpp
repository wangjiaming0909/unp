#include "Handler.h"
#include "http/gperf/HttpHeaderCode.h"
#include "util/easylogging++.h"
#include <utility>

namespace downloader
{

Handler::Handler(reactor::Reactor& react, const std::string& url, bool isSSL) 
    : connection_handler(react, isSSL)
    , request_{}
    , codec_{http::HttpDirection::DOWNSTREAM}
    , urlParser_{}
    , url_{url}
    , fileWriterPtr_{nullptr}
{
    codec_.setCallback(this);
    urlParser_.init(url_);
    if (!urlParser_.valid())
    {
        url_ = "";
        LOG(WARNING) << "url format error...";
        return;
    }
}

int Handler::handle_input(int handle)
{
    auto ret = connection_handler::handle_input(handle);
    if(ret < 0)
    {
        LOG(ERROR) << "error in handle_input...";
        return -1;
    }
    if(input_buffer_.buffer_length() == 0)
    {
        LOG(WARNING) << "no data, retrying...";
        if(isShouldClose_)
            return -1;
        return 0;
    }

    while(input_buffer_.buffer_length() > 0)
    {
        auto firstChain = input_buffer_.begin().chain();
        auto data = firstChain.get_start_buffer();
        auto chainLen = firstChain.size();
        if(data != nullptr && chainLen != 0)
        {
            std::string s{static_cast<char*>(data), chainLen};
            LOG(INFO) << "----------------received: " << s;
        }
        string_piece::const_string_piece sp{static_cast<const char*>(data), chainLen};
        size_t bytesRead = codec_.onIngress(sp);
        if(codec_.hasError())
        {
            LOG(WARNING) << "Http parser parse error";
            codec_.pause(1);
            return -1;
        }
        assert(bytesRead == chainLen);
        input_buffer_.drain(chainLen);
    }
    if(isShouldClose_) 
        return -1;
    return 0;
}

int Handler::open()
{
    request_.setHttpVersion(1, 1);
    request_.setRequestPath(urlParser_.path().cbegin());
    request_.setRequestMethod(http::HTTPMethod::GET);
    request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_HOST, urlParser_.host());
    request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_USER_AGENT, USERAGENT);
    request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT, ACCEPT);
    request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING, ACCEPTENCODING);
    if(usingRangeDownload_)
    {
        request_.addHeader(
            http::HttpHeaderCode::HTTP_HEADER_RANGE, 
            "range: bytes=" 
            + std::to_string(rangeBegin_) 
            + "-" 
            + std::to_string(rangeEnd_));
    }
    auto messageStr = request_.buildRequestMessage();
    LOG(INFO) << *messageStr;
    auto bytesWritten = write(messageStr->c_str(), messageStr->size());
    if (bytesWritten <= 0) return -1;
    status_ = HandlerStatus::REQUEST_SENT;

    return enable_reading();
}

int Handler::onStatus(const char* /*buf*/, size_t/* len*/)
{
    switch(codec_.status())
	{
		case 302:
			status_ = HandlerStatus::RECEIVED302;
			return 0;
		case 200:
			status_ = HandlerStatus::RECEIVED200;
			return 0;
		default:
			codec_.pause(1);
			return -1;
	}
}

int Handler::onBody(const char* buf, size_t size)
{
	if(status_ == HandlerStatus::CHUNK_ENCODING 
            || status_ == HandlerStatus::NOT_RESPONDING_TO_RANGE)
    {
        bytesDownloaded_ += size;
        if(!fileWriterPtr_) return 0;
        fileWriterPtr_->write(buf, size);
        fileWriterPtr_->flush();
        //LOG(INFO) << "CHUNK_ENCODING With on body size: " << size;
        return 0;
        //bytesDownloaded_ += size;
    }

    if(status_ == HandlerStatus::RANGE_MATCH)
    {
        bytesDownloaded_ += size;
        if(!fileWriterPtr_) return 0;
        fileWriterPtr_->write(buf, size);
        fileWriterPtr_->flush();
        return 0;
    }
    
    if(status_ == HandlerStatus::RANGE_NOT_MATCH)
    {
        isShouldClose_ = true;
        return -1;
    }
    return 0;
}

int Handler::onHeadersComplete(size_t /*len*/)
{
	if(status_ == HandlerStatus::RECEIVED302)
	{
		auto* locationHeader = codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_LOCATION);
		if(locationHeader == nullptr) 
		{
			LOG(INFO) << "server returned 302, but no location header...";
			isShouldClose_ = true;
			status_ = HandlerStatus::RECV302_WITHOUT_NEW_LOCATION;
			return -1;
		}
		url_ = *locationHeader;
		status_ = HandlerStatus::NEW_LOCATION_GOT;
		isShouldClose_ = true;
        return -1;
	}

    if(status_ == HandlerStatus::RECEIVED200)
	{
		auto* cd = codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_CONTENT_DISPOSITION);
		if(cd == nullptr) 
		{
			LOG(WARNING) << "no HTTP_HEADER_CONTENT_DISPOSITION heade...";
			status_ = HandlerStatus::NO_CONTENT_DISPOSITION;
			return -1;
		}

		retriveFileNameFromContentDisposition(*cd);
        initFileWriter();

		if(usingRangeDownload_)
		{
			if(*codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_TRANSFER_ENCODING) == "chunked")
			{
				status_ = HandlerStatus::CHUNK_ENCODING;
				return 0;
			}
			auto* contentRangeHeader = codec_.message().getHeaderValue(http::HttpHeaderCode::HTTP_HEADER_CONTENT_RANGE);
			if(contentRangeHeader == nullptr)
			{
				LOG(ERROR) << "trying to get data with range header, but server didn't respond with range...";
				status_ = HandlerStatus::NOT_RESPONDING_TO_RANGE;
				return 0;
			}
			auto pair = parseContentRangeHeader(*contentRangeHeader);
			if(pair.first != rangeBegin_ && pair.second != rangeEnd_)
			{
				LOG(ERROR) << "Range error preset begin: " << rangeBegin_ 
                           << " end: " << rangeEnd_ 
                           << " got begin: " << pair.first 
                           << " end: " << pair.second;
				status_ = HandlerStatus::RANGE_NOT_MATCH;
				return -1;
			}
			status_ = HandlerStatus::RANGE_MATCH;
		}
	}
	return 0;
}

std::pair<uint64_t, uint64_t> Handler::parseContentRangeHeader(const std::string& headerValue)
{
	return std::make_pair(rangeBegin_, rangeEnd_);
}

void Handler::retriveFileNameFromContentDisposition(const std::string& cd)
{
    auto it = std::find(cd.begin(), cd.end(), '=');
    if(it == cd.end())
    {
        LOG(WARNING) << "Unknown content disposition...";
        fileName_ = DEFAULT_FILE_NAME;
        return;
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

int Handler::onHeaderField(const char * /*buf*/, size_t /*len*/)
{
    return 0;
}

int Handler::onHeaderValue(const char * /*buf*/, size_t /*len*/)
{
    return 0;
}

int Handler::onMessageComplete()
{
    LOG(INFO) << "message completed... ";
    isShouldClose_ = true;
    return 0;
}

int Handler::onChunkHeader(size_t len)
{
    LOG(INFO) << "On chunked header len: " << len;
    return 0;
}

int Handler::onChunkComplete()
{
    LOG(INFO) << "chunk completed...";
    return 0;
}
} 
