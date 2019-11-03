#include "HttpMethod.h"
#include <string>
#include <vector>

namespace http
{

#define HTTP_METHOD_STRING(method) #method

const std::vector<std::string> getHttpMethodString()
{
    static const std::vector<std::string>* httpMethodStrings = 
    new std::vector<std::string>
    {
        HTTP_METHOD_GEN(HTTP_METHOD_STRING)
    };
    return *httpMethodStrings;
}

boost::optional<HTTPMethod> stringToMethod(string_piece::const_string_piece methodName)
{
    const auto& methodStrings = getHttpMethodString();
    for(size_t i = 0; i < methodStrings.size(); i++)
    {
        if(methodName.caseInsensitiveEqual(methodStrings[i].c_str()))
        {
            return HTTPMethod(i);
        }
    }
    return boost::none;
}

const char* methodToString(HTTPMethod method)
{
    switch(method)
    {
        case HTTPMethod::GET:
            return "GET";
        case HTTPMethod::POST:
            return "POST";
        default:
            return "";
    }
}
}