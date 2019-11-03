#include <gtest/gtest.h>
#include "http/HttpMessage.h"
#include "util/easylogging++.h"

string_piece::const_string_piece cookie = "PANWEB=1; Hm_lvt_7a3960b6f067eb0085b7f96ff5e660b0=1572450095,1572517934,1572615782,1572749718; pan_login_way=1; STOKEN=a4d24c8f068da3e11daf01be54c520b6f8203ca383eb72992ceb489e5ee57c6d; SCRC=4745f44cdc7a6a4662514271aa3b9945; BDCLND=Wqf8ANaeS%2FgdzlmZllURan%2FqgQMnkhfGKsk6RZV73%2F8%3D; BAIDUID=422D40C4C04A5CB5F751B6D0D3AD7C69:FG=1; BIDUPSID=BEC22DC0373ACF912C2806F859857040; PSTM=1570446595; MCITY=-131%3A; BDUSS=FpuRElJZHFla2ZUUmFUckVlOFFLcXdjWjZQRkVhYWFuLTdyMWo5Yks5VWd5ZU5kRUFBQUFBJCQAAAAAAAAAAAEAAACs7Akhva27p7SoMTAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA8vF0gPLxdb; BDORZ=B490B5EBF6F3CD402E515D22BCDA1598; H_PS_PSSID=1465_21107_29073_29910_29988_29568_29700_29220; BDRCVFR[feWj1Vr5u3D]=I67x6TjHwwYf0; delPer=0; PSINO=2; ZD_ENTRY=baidu; PANPSC=18206564409029913488%3ADJI9ZdfpjgINl6LR6xndZZgtJeEFa7WQZqmb8L1pKxQ3I2qzP1oavCHLZeHZ%2FVI0COfgdHnp14%2B5uNhL25C0QPGEfNx8mgFTq1kZVdV54dNAvJzt0MGlV2T%2FMK3iZvOoYLtmzkRm4Oei%2BY1X7udr9G2gud9gLjYtpqvB0tQPnfltcQe%2BbgwEaTCo8bcT1HMK";

string_piece::const_string_piece host = "www.baidu.com";
string_piece::const_string_piece userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:69.0) Gecko/20100101 Firefox/69.0";

TEST(HttpMessage, normal)
{
    using namespace http;
    HttpMessage request{};
    request.setHttpVersion(1, 1);
    request.setRequestMethod(http::HTTPMethod::GET);
    request.setRequestPath("/");
    request.addHeader(http::HttpHeaderCode::HTTP_HEADER_HOST, host);
    request.addHeader(http::HttpHeaderCode::HTTP_HEADER_USER_AGENT, userAgent);
    request.addHeader(http::HttpHeaderCode::HTTP_HEADER_COOKIE, cookie);
    auto messageStr = request.buildRequestMessage();
    LOG(INFO) << *messageStr;
}