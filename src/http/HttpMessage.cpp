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

}//namespace http