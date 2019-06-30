#include "URLParser.h"

namespace http
{

static bool validateScheme(string_piece::const_string_piece url)
{
    string_piece::const_string_piece tmp = "://";
    auto schemeEnd = url.find(tmp);
    if(schemeEnd == -1)
    {
        return false;
    }
    auto scheme = url.sub_string(0, schemeEnd);
    return std::all_of(scheme.begin(), scheme.end(), [](auto _) {
        return std::isalpha(_);
    });
}

URLParser::~URLParser()
{
}

void URLParser::init(const_string_piece_t urlVal)
{
    if(initilized_)
        return;
    url_ = urlVal;
    parse();
    initilized_ = true;
}

void URLParser::parse()
{
    if(!validateScheme(url_)) return;

    struct http_parser_url u;
    memset(&u, 0, sizeof(struct http_parser_url));

    valid_ = !http_parser_parse_url(url_.begin(), url_.size(), 0, &u);
    if(!valid_) return;

    scheme_ = url_.sub_string(u.field_data[UF_SCHEMA].off,
                              u.field_data[UF_SCHEMA].len);

    host_ = url_.sub_string(u.field_data[UF_HOST].off,
                            u.field_data[UF_HOST].len);

    port_ = u.port;

    path_ = url_.sub_string(u.field_data[UF_PATH].off,
                            u.field_data[UF_PATH].len);

    query_ = url_.sub_string(u.field_data[UF_QUERY].off,
                             u.field_data[UF_QUERY].len);
}
}
