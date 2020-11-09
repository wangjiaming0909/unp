#include "HttpMessage.h"

namespace http
{

HttpMessage::HttpMessage()
    : startTime_(util::Time::now())
{
    // if(isRequest) message_ = HttpRequest();
    // else message_ = HttpResponse();
}

HttpMessage::~HttpMessage()
{

}

const HttpMessage::string_t HttpMessage::getVersionStr()
{
	if(version_.first == 0 && version_.second == 0)
	{
		return "";
	}
	if(versionStr_.size() > 0) return std::string("HTTP/").append(versionStr_);
	auto format = boost::format("HTTP/%d.%d") % version_.first % version_.second;
	return format.str();
}

std::shared_ptr<std::string> HttpMessage::buildRequestLine()
{
	if(message_.which() == 2) return nullptr;
	std::shared_ptr<std::string> reqLine = std::make_shared<std::string>();
	reqLine->append(methodToString(request().method_)).append(" ");
	reqLine->append(request().path_).append(" ");
	reqLine->append(getVersionStr()).append("\r\n");
	return reqLine;
}

    bool HttpMessage::hasHeader(HttpHeaderCode code) const
    {
        auto value = headers_[code];
        return value != nullptr;
    }

}//namespace http
