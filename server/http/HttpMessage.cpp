#include "HttpMessage.h"

namespace http
{

HttpMessage::HttpMessage() : startTime_(util::Time::now())
{
}

HttpMessage::~HttpMessage()
{
}

}//namespace http