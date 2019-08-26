#include "HttpParser.h"

namespace http
{


HttpPaser::HttpPaser(http_parser_type parserType)
    : parser_{}
    , parserSettings_{}
{
    http_parser_init(&parser_, parserType);
}

HttpPaser::~HttpPaser()
{
}


}
