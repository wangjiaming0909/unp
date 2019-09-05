#include "HttpParser.h"

namespace http
{


HttpPaser::HttpPaser(http_parser_type parserType)
    : parserSettings_{}
    , parser_{}
{
    http_parser_init(&parser_, parserType);
}

HttpPaser::~HttpPaser()
{
}


}
