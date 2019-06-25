#ifndef _HTTP_COMMOM_HEADERS_HASH_H_
#define _HTTP_COMMOM_HEADERS_HASH_H_
#include <cstdint>
#include <cstring>
#include "HttpHeaderCode.h"

namespace http
{
struct HttpHeader
{
    const char* headerName;
    HttpHeaderCode headerCode;
};

#define TOTAL_KEYWORDS 93
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 37
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 148
/* maximum key range = 146, duplicates = 0 */

class HttpCommomHeaderInternal
{
private:
  static inline unsigned int hash (const char *str, size_t len);
public:
  static struct HttpHeader *isValidHttpHeader (const char *str, size_t len);
};


static struct HttpHeader wordlist[] =
  {
#line 67 "HttpCommomHeadersHash.h.gperf"
    {"P3P", HttpHeaderCode::HTTP_HEADER_P3P},
#line 55 "HttpCommomHeadersHash.h.gperf"
    {"Host", HttpHeaderCode::HTTP_HEADER_HOST},
#line 34 "HttpCommomHeadersHash.h.gperf"
    {"Allow", HttpHeaderCode::HTTP_HEADER_ALLOW},
#line 19 "HttpCommomHeadersHash.h.gperf"
    {"Accept", HttpHeaderCode::HTTP_HEADER_ACCEPT},
#line 35 "HttpCommomHeadersHash.h.gperf"
    {"Alt-Svc", HttpHeaderCode::HTTP_HEADER_ALT_SVC},
#line 51 "HttpCommomHeadersHash.h.gperf"
    {"Expect", HttpHeaderCode::HTTP_HEADER_EXPECT},
#line 52 "HttpCommomHeadersHash.h.gperf"
    {"Expires", HttpHeaderCode::HTTP_HEADER_EXPIRES},
#line 33 "HttpCommomHeadersHash.h.gperf"
    {"Age", HttpHeaderCode::HTTP_HEADER_AGE},
#line 20 "HttpCommomHeadersHash.h.gperf"
    {"Accept-Charset", HttpHeaderCode::HTTP_HEADER_ACCEPT_CHARSET},
#line 38 "HttpCommomHeadersHash.h.gperf"
    {"Connection", HttpHeaderCode::HTTP_HEADER_CONNECTION},
#line 47 "HttpCommomHeadersHash.h.gperf"
    {"Cookie", HttpHeaderCode::HTTP_HEADER_COOKIE},
#line 73 "HttpCommomHeadersHash.h.gperf"
    {"Referer", HttpHeaderCode::HTTP_HEADER_REFERER},
#line 24 "HttpCommomHeadersHash.h.gperf"
    {"Accept-Ranges", HttpHeaderCode::HTTP_HEADER_ACCEPT_RANGES},
#line 72 "HttpCommomHeadersHash.h.gperf"
    {"Range", HttpHeaderCode::HTTP_HEADER_RANGE},
#line 43 "HttpCommomHeadersHash.h.gperf"
    {"Content-Location", HttpHeaderCode::HTTP_HEADER_CONTENT_LOCATION},
#line 46 "HttpCommomHeadersHash.h.gperf"
    {"Content-Type", HttpHeaderCode::HTTP_HEADER_CONTENT_TYPE},
#line 45 "HttpCommomHeadersHash.h.gperf"
    {"Content-Range", HttpHeaderCode::HTTP_HEADER_CONTENT_RANGE},
#line 39 "HttpCommomHeadersHash.h.gperf"
    {"Content-Disposition", HttpHeaderCode::HTTP_HEADER_CONTENT_DISPOSITION},
#line 61 "HttpCommomHeadersHash.h.gperf"
    {"Keep-Alive", HttpHeaderCode::HTTP_HEADER_KEEP_ALIVE},
#line 41 "HttpCommomHeadersHash.h.gperf"
    {"Content-Language", HttpHeaderCode::HTTP_HEADER_CONTENT_LANGUAGE},
#line 26 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Allow-Headers", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_ALLOW_HEADERS},
#line 29 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Expose-Headers", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_EXPOSE_HEADERS},
#line 31 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Request-Headers", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_REQUEST_HEADERS},
#line 78 "HttpCommomHeadersHash.h.gperf"
    {"Server", HttpHeaderCode::HTTP_HEADER_SERVER},
#line 25 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Allow-Credentials", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_ALLOW_CREDENTIALS},
#line 59 "HttpCommomHeadersHash.h.gperf"
    {"If-Range", HttpHeaderCode::HTTP_HEADER_IF_RANGE},
#line 100 "HttpCommomHeadersHash.h.gperf"
    {"X-Real-IP", HttpHeaderCode::HTTP_HEADER_X_REAL_IP},
#line 98 "HttpCommomHeadersHash.h.gperf"
    {"X-Frame-Options", HttpHeaderCode::HTTP_HEADER_X_FRAME_OPTIONS},
#line 93 "HttpCommomHeadersHash.h.gperf"
    {"X-Accel-Redirect", HttpHeaderCode::HTTP_HEADER_X_ACCEL_REDIRECT},
#line 74 "HttpCommomHeadersHash.h.gperf"
    {"Refresh", HttpHeaderCode::HTTP_HEADER_REFRESH},
#line 88 "HttpCommomHeadersHash.h.gperf"
    {"VIP", HttpHeaderCode::HTTP_HEADER_VIP},
#line 42 "HttpCommomHeadersHash.h.gperf"
    {"Content-Length", HttpHeaderCode::HTTP_HEADER_CONTENT_LENGTH},
#line 68 "HttpCommomHeadersHash.h.gperf"
    {"Pragma", HttpHeaderCode::HTTP_HEADER_PRAGMA},
#line 30 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Max-Age", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_MAX_AGE},
#line 58 "HttpCommomHeadersHash.h.gperf"
    {"If-None-Match", HttpHeaderCode::HTTP_HEADER_IF_NONE_MATCH},
#line 50 "HttpCommomHeadersHash.h.gperf"
    {"ETag", HttpHeaderCode::HTTP_HEADER_ETAG},
#line 96 "HttpCommomHeadersHash.h.gperf"
    {"X-Forwarded-For", HttpHeaderCode::HTTP_HEADER_X_FORWARDED_FOR},
#line 105 "HttpCommomHeadersHash.h.gperf"
    {"X-XSS-Protection", HttpHeaderCode::HTTP_HEADER_X_XSS_PROTECTION},
#line 95 "HttpCommomHeadersHash.h.gperf"
    {"X-Content-Type-Options", HttpHeaderCode::HTTP_HEADER_X_CONTENT_TYPE_OPTIONS},
#line 104 "HttpCommomHeadersHash.h.gperf"
    {"X-Wap-Profile", HttpHeaderCode::HTTP_HEADER_X_WAP_PROFILE},
#line 70 "HttpCommomHeadersHash.h.gperf"
    {"Proxy-Authorization", HttpHeaderCode::HTTP_HEADER_PROXY_AUTHORIZATION},
#line 79 "HttpCommomHeadersHash.h.gperf"
    {"Set-Cookie", HttpHeaderCode::HTTP_HEADER_SET_COOKIE},
#line 40 "HttpCommomHeadersHash.h.gperf"
    {"Content-Encoding", HttpHeaderCode::HTTP_HEADER_CONTENT_ENCODING},
#line 82 "HttpCommomHeadersHash.h.gperf"
    {"TE", HttpHeaderCode::HTTP_HEADER_TE},
#line 69 "HttpCommomHeadersHash.h.gperf"
    {"Proxy-Authenticate", HttpHeaderCode::HTTP_HEADER_PROXY_AUTHENTICATE},
#line 63 "HttpCommomHeadersHash.h.gperf"
    {"Link", HttpHeaderCode::HTTP_HEADER_LINK},
#line 22 "HttpCommomHeadersHash.h.gperf"
    {"Accept-Encoding", HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING},
#line 44 "HttpCommomHeadersHash.h.gperf"
    {"Content-MD5", HttpHeaderCode::HTTP_HEADER_CONTENT_MD5},
#line 84 "HttpCommomHeadersHash.h.gperf"
    {"Trailer", HttpHeaderCode::HTTP_HEADER_TRAILER},
#line 64 "HttpCommomHeadersHash.h.gperf"
    {"Location", HttpHeaderCode::HTTP_HEADER_LOCATION},
#line 49 "HttpCommomHeadersHash.h.gperf"
    {"Date", HttpHeaderCode::HTTP_HEADER_DATE},
#line 77 "HttpCommomHeadersHash.h.gperf"
    {"Sec-WebSocket-Accept", HttpHeaderCode::HTTP_HEADER_SEC_WEBSOCKET_ACCEPT},
#line 101 "HttpCommomHeadersHash.h.gperf"
    {"X-Requested-With", HttpHeaderCode::HTTP_HEADER_X_REQUESTED_WITH},
#line 17 "HttpCommomHeadersHash.h.gperf"
    {":Status", HttpHeaderCode::HTTP_HEADER_COLON_STATUS},
#line 56 "HttpCommomHeadersHash.h.gperf"
    {"If-Match", HttpHeaderCode::HTTP_HEADER_IF_MATCH},
#line 32 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Request-Method", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_REQUEST_METHOD},
#line 23 "HttpCommomHeadersHash.h.gperf"
    {"Accept-Language", HttpHeaderCode::HTTP_HEADER_ACCEPT_LANGUAGE},
#line 71 "HttpCommomHeadersHash.h.gperf"
    {"Proxy-Connection", HttpHeaderCode::HTTP_HEADER_PROXY_CONNECTION},
#line 102 "HttpCommomHeadersHash.h.gperf"
    {"X-Thrift-Protocol", HttpHeaderCode::HTTP_HEADER_X_THRIFT_PROTOCOL},
#line 36 "HttpCommomHeadersHash.h.gperf"
    {"Authorization", HttpHeaderCode::HTTP_HEADER_AUTHORIZATION},
#line 60 "HttpCommomHeadersHash.h.gperf"
    {"If-Unmodified-Since", HttpHeaderCode::HTTP_HEADER_IF_UNMODIFIED_SINCE},
#line 21 "HttpCommomHeadersHash.h.gperf"
    {"Accept-Datetime", HttpHeaderCode::HTTP_HEADER_ACCEPT_DATETIME},
#line 16 "HttpCommomHeadersHash.h.gperf"
    {":Scheme", HttpHeaderCode::HTTP_HEADER_COLON_SCHEME},
#line 90 "HttpCommomHeadersHash.h.gperf"
    {"Via", HttpHeaderCode::HTTP_HEADER_VIA},
#line 89 "HttpCommomHeadersHash.h.gperf"
    {"Vary", HttpHeaderCode::HTTP_HEADER_VARY},
#line 103 "HttpCommomHeadersHash.h.gperf"
    {"X-UA-Compatible", HttpHeaderCode::HTTP_HEADER_X_UA_COMPATIBLE},
#line 99 "HttpCommomHeadersHash.h.gperf"
    {"X-Powered-By", HttpHeaderCode::HTTP_HEADER_X_POWERED_BY},
#line 54 "HttpCommomHeadersHash.h.gperf"
    {"Front-End-Https", HttpHeaderCode::HTTP_HEADER_FRONT_END_HTTPS},
#line 80 "HttpCommomHeadersHash.h.gperf"
    {"Sec-Token-Binding", HttpHeaderCode::HTTP_HEADER_SEC_TOKEN_BINDING},
#line 83 "HttpCommomHeadersHash.h.gperf"
    {"Timestamp", HttpHeaderCode::HTTP_HEADER_TIMESTAMP},
#line 15 "HttpCommomHeadersHash.h.gperf"
    {":Path", HttpHeaderCode::HTTP_HEADER_COLON_PATH},
#line 97 "HttpCommomHeadersHash.h.gperf"
    {"X-Forwarded-Proto", HttpHeaderCode::HTTP_HEADER_X_FORWARDED_PROTO},
#line 37 "HttpCommomHeadersHash.h.gperf"
    {"Cache-Control", HttpHeaderCode::HTTP_HEADER_CACHE_CONTROL},
#line 76 "HttpCommomHeadersHash.h.gperf"
    {"Sec-WebSocket-Key", HttpHeaderCode::HTTP_HEADER_SEC_WEBSOCKET_KEY},
#line 48 "HttpCommomHeadersHash.h.gperf"
    {"DNT", HttpHeaderCode::HTTP_HEADER_DNT},
#line 53 "HttpCommomHeadersHash.h.gperf"
    {"From", HttpHeaderCode::HTTP_HEADER_FROM},
#line 75 "HttpCommomHeadersHash.h.gperf"
    {"Retry-After", HttpHeaderCode::HTTP_HEADER_RETRY_AFTER},
#line 14 "HttpCommomHeadersHash.h.gperf"
    {":Method", HttpHeaderCode::HTTP_HEADER_COLON_METHOD},
#line 13 "HttpCommomHeadersHash.h.gperf"
    {":Authority", HttpHeaderCode::HTTP_HEADER_COLON_AUTHORITY},
#line 66 "HttpCommomHeadersHash.h.gperf"
    {"Origin", HttpHeaderCode::HTTP_HEADER_ORIGIN},
#line 85 "HttpCommomHeadersHash.h.gperf"
    {"Transfer-Encoding", HttpHeaderCode::HTTP_HEADER_TRANSFER_ENCODING},
#line 57 "HttpCommomHeadersHash.h.gperf"
    {"If-Modified-Since", HttpHeaderCode::HTTP_HEADER_IF_MODIFIED_SINCE},
#line 92 "HttpCommomHeadersHash.h.gperf"
    {"Warning", HttpHeaderCode::HTTP_HEADER_WARNING},
#line 86 "HttpCommomHeadersHash.h.gperf"
    {"Upgrade", HttpHeaderCode::HTTP_HEADER_UPGRADE},
#line 87 "HttpCommomHeadersHash.h.gperf"
    {"User-Agent", HttpHeaderCode::HTTP_HEADER_USER_AGENT},
#line 91 "HttpCommomHeadersHash.h.gperf"
    {"WWW-Authenticate", HttpHeaderCode::HTTP_HEADER_WWW_AUTHENTICATE},
#line 28 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Allow-Origin", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN},
#line 62 "HttpCommomHeadersHash.h.gperf"
    {"Last-Modified", HttpHeaderCode::HTTP_HEADER_LAST_MODIFIED},
#line 94 "HttpCommomHeadersHash.h.gperf"
    {"X-Content-Security-Policy-Report-Only", HttpHeaderCode::HTTP_HEADER_X_CONTENT_SECURITY_POLICY_REPORT_ONLY},
#line 81 "HttpCommomHeadersHash.h.gperf"
    {"Strict-Transport-Security", HttpHeaderCode::HTTP_HEADER_STRICT_TRANSPORT_SECURITY},
#line 65 "HttpCommomHeadersHash.h.gperf"
    {"Max-Forwards", HttpHeaderCode::HTTP_HEADER_MAX_FORWARDS},
#line 18 "HttpCommomHeadersHash.h.gperf"
    {":Protocol", HttpHeaderCode::HTTP_HEADER_COLON_PROTOCOL},
#line 27 "HttpCommomHeadersHash.h.gperf"
    {"Access-Control-Allow-Methods", HttpHeaderCode::HTTP_HEADER_ACCESS_CONTROL_ALLOW_METHODS}
  };
}

#endif //_HTTP_COMMOM_HEADERS_HASH_H_