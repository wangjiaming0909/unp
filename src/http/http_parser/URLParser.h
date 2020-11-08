#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include <cstdint>
#include "util/string_piece/string_piece.h"
#include "http_parser.h"

namespace http
{

class URLParser
{
public:
    using const_string_piece_t = string_piece::const_string_piece;
    using string_piece_t = string_piece::mutable_string_piece;

public:
    URLParser(){}
    explicit URLParser(const_string_piece_t urlVal) noexcept
    {
        init(urlVal);
    }
    ~URLParser();

    void init(const_string_piece_t urlVal);
    void reset(const_string_piece_t url);

    const const_string_piece_t& url() const { return url_; }
    const const_string_piece_t& scheme() const { return scheme_; }
    bool is_https() const { return scheme_ == "https";}
    const const_string_piece_t& host() const {return host_;}
    uint16_t port() const {return port_;}
    const const_string_piece_t& path() const {return path_;}
    const const_string_piece_t& query() const {return query_;}
    bool valid() const {return valid_;}

private:
    void parse();

private:
    const_string_piece_t        url_;
    const_string_piece_t        scheme_;
    const_string_piece_t        host_;
    const_string_piece_t        path_;
    const_string_piece_t        query_;
    uint16_t                    port_{0};
    bool                        valid_{false};
    bool                        initilized_{false};
};


}

#endif //_HTTP_PARSER_H_
