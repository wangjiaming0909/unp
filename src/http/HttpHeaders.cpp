#include "HttpHeaders.h"
#include <exception>

namespace http{

HttpHeaders::HttpHeaders()
    : codes_{}
    , headerNames_{}
    , headerValues_{}
    , codes_deleted_{0}
{
    codes_.reserve(INIT_VECTOR_RESERVE_SIZE);
    headerNames_.reserve(INIT_VECTOR_RESERVE_SIZE);
    headerValues_.reserve(INIT_VECTOR_RESERVE_SIZE);
}

HttpHeaders::HttpHeaders(const HttpHeaders& other)
    : codes_{other.codes_}
    , headerNames_{other.headerNames_}
    , headerValues_{other.headerValues_}
    , codes_deleted_{other.codes_deleted_}
{
    for(size_t i = 0; i < codes_.size(); i++)
    {
        size_t len = strlen(other.headerNames_[i]);
        char* p = static_cast<char*>(::calloc(len + 1, 1));
        ::memcpy(p, other.headerNames_[i], len);
        if(codes_[i] == HttpHeaderCode::HTTP_HEADER_OTHER)
            headerNames_[i] = p;
    }
}

HttpHeaders::HttpHeaders(HttpHeaders&& other)
    : codes_{std::move(other.codes_)}
    , headerNames_{std::move(other.headerNames_)}
    , headerValues_{std::move(other.headerValues_)}
    , codes_deleted_{other.codes_deleted_}
{
    other.clearAll();
}

HttpHeaders& HttpHeaders::operator=(const HttpHeaders& other)
{
    (void)other;
    throw new std::exception();
}

HttpHeaders& HttpHeaders::operator=(HttpHeaders&& other)
{
    (void)other;
    throw new std::exception();
}

HttpHeaders::~HttpHeaders()
{
    disposeHeaderNames();
}

void HttpHeaders::add(const_string_piece headerName, const_string_piece headerValue)
{
    CHECK_SIZE(headerName.size())
    // CHECK_SIZE(headerValue.size())

    auto code = HttpCommomHeaders::getHeaderCode(headerName.cbegin(), headerName.size());
    codes_.push_back(code);

    headerNames_.push_back(
        code == HttpHeaderCode::HTTP_HEADER_OTHER ? copyFrom(headerName) : HttpCommomHeaders::getPointerWithHeaderCode(code)
    );
    headerValues_.emplace_back(headerValue.cbegin(), headerValue.size());
}

void HttpHeaders::add(const std::string& headerName, const std::string& headerValue)
{
    CHECK_SIZE(headerName.size())
    // CHECK_SIZE(headerValue.size())

    auto code = HttpCommomHeaders::getHeaderCode(headerName);
    codes_.push_back(code);
    headerNames_.push_back(
        code == HttpHeaderCode::HTTP_HEADER_OTHER ? copyFrom(headerName) : HttpCommomHeaders::getPointerWithHeaderCode(code)
    );
    headerValues_.emplace_back(headerValue);
}

void HttpHeaders::add(const std::string& headerName, std::string&& headerValue)
{
    CHECK_SIZE(headerName.size())
    // CHECK_SIZE(headerValue.size())

    auto code = HttpCommomHeaders::getHeaderCode(headerName);
    codes_.push_back(code);
    headerNames_.push_back(
        code == HttpHeaderCode::HTTP_HEADER_OTHER ? copyFrom(headerName) : HttpCommomHeaders::getPointerWithHeaderCode(code)
    );
    headerValues_.emplace_back(std::move(headerValue));
}

void HttpHeaders::add(HttpHeaderCode code, std::string&& headerValue)
{
    if(code == HttpHeaderCode::HTTP_HEADER_NONE || code == HttpHeaderCode::HTTP_HEADER_OTHER)
        return;

    codes_.push_back(code);
    headerNames_.push_back(HttpCommomHeaders::getPointerWithHeaderCode(code));
    headerValues_.emplace_back(std::move(headerValue));
}

void HttpHeaders::add(HttpHeaderCode code, const_string_piece headerValue)
{
    if(code == HttpHeaderCode::HTTP_HEADER_NONE || code == HttpHeaderCode::HTTP_HEADER_OTHER)
        return;

    std::string value{headerValue.cbegin(), headerValue.size()};
    codes_.push_back(code);
    headerNames_.push_back(HttpCommomHeaders::getPointerWithHeaderCode(code));
    headerValues_.emplace_back(std::move(value));
}

bool HttpHeaders::removeWithStdString(const std::string& headerName)
{
    return remove(headerName);
}

bool HttpHeaders::remove(const_string_piece headerName)
{
    bool removed = false;
    if(headerName.size() == 0) return false;
    auto code = HttpCommomHeaders::getHeaderCode(headerName.cbegin(), headerName.size());
    if(code != HttpHeaderCode::HTTP_HEADER_OTHER)
    {
        return remove(code);
    }
    else
    {
        iterateOverOtherHeaderNames(headerName,
            [&](
                std::vector<HttpHeaderCode>& codes,
                std::vector<const char*>&,
                std::vector<std::string>&,
                size_t& codes_deleted, 
                size_t pos)
            {
                codes[pos] = HttpHeaderCode::HTTP_HEADER_NONE;
                codes_deleted++;
                removed = true;
            }
        );
    }
    return removed;
}

bool HttpHeaders::remove(HttpHeaderCode code)
{
    bool remvoed = false;
    iterateOverCodes(code, 
        [&](
            std::vector<HttpHeaderCode>& codes,
            std::vector<const char*>&,
            std::vector<std::string>&,
            size_t& codes_deleted, 
            size_t pos)
        {
            codes[pos] = HttpHeaderCode::HTTP_HEADER_NONE;
            codes_deleted++;
            remvoed = true;
        }
    );
    return remvoed;
}

void HttpHeaders::clearAll()
{
    disposeHeaderNames();

    codes_.clear();
    headerNames_.clear();
    headerValues_.clear();
    codes_deleted_ = 0;
}

void HttpHeaders::disposeHeaderNames()
{
    for(size_t i = 0; i < codes_.size(); i++)
    {
        if(HttpHeaderCode::HTTP_HEADER_OTHER == codes_[i])
            ::free(const_cast<void*>(static_cast<const void*>(headerNames_[i])));
    }
}

std::shared_ptr<std::string> HttpHeaders::dump() const
{
    std::shared_ptr<std::string> ret = std::make_shared<std::string>();
    if(headerNames_.size() == 0) return ret;
    if(headerNames_.size() != headerValues_.size()) return ret;
    for (size_t i = 0; i < headerNames_.size(); i++)
    {
        ret->append(headerNames_[i]).append(": ").append(headerValues_[i]).append("\n");
    }
    ret->append("\n\n");
}

const size_t HttpHeaders::INIT_VECTOR_RESERVE_SIZE = 16;

char *copyFrom(string_piece::const_string_piece str)
{
    return str.copy();
}

char *copyFrom(const std::string &str)
{
    if(str.size() == 0)
        return 0;
    char *data = static_cast<char *>(::calloc(str.size() + 1, 1));
    ::memcpy(data, str.data(), str.size());
    return data;
}

void __()
{
    (void)wordlist;
}

}
