#include <gtest/gtest.h>
#include "http/HttpCommomHeaders.h"

using namespace http;

namespace http_test
{

void _1()
{
    (void)wordlist;
}

TEST(http_commom_headers, test_getHeaderCode_char_pointer){
    const char* headerName = ":Authority";
    auto ret = HttpCommomHeaders::getHeaderCode(headerName, strlen(headerName));
    ASSERT_EQ(ret, HttpHeaderCode::HTTP_HEADER_COLON_AUTHORITY);

    headerName = "Date";
    ret = HttpCommomHeaders::getHeaderCode(headerName, strlen(headerName));
    ASSERT_EQ(ret, HttpHeaderCode::HTTP_HEADER_DATE);

    headerName = "none";
    ret = HttpCommomHeaders::getHeaderCode(headerName, strlen(headerName));
    ASSERT_EQ(ret, HttpHeaderCode::HTTP_HEADER_OTHER);

    headerName = ":Method";
    ret = HttpCommomHeaders::getHeaderCode(headerName, strlen(headerName));
    ASSERT_EQ(ret, HttpHeaderCode::HTTP_HEADER_COLON_METHOD);
}

TEST(http_commom_headers, test_getHeaderCode_string){
    std::string headerName = "Keep-Alive";
    auto ret = HttpCommomHeaders::getHeaderCode(headerName);
    ASSERT_EQ(ret, HttpHeaderCode::HTTP_HEADER_KEEP_ALIVE);

    headerName = "none";
    ret = HttpCommomHeaders::getHeaderCode(headerName);
    ASSERT_EQ(ret, HttpHeaderCode::HTTP_HEADER_OTHER);
}

TEST(http_commom_headers, test_getPointerWithHeaderCode)
{
    HttpHeaderCode code = HttpHeaderCode::HTTP_HEADER_KEEP_ALIVE;
    auto ret = HttpCommomHeaders::getPointerWithHeaderCode(code);
    ASSERT_STREQ(ret, "Keep-Alive");

    auto ret2 = HttpCommomHeaders::getPointerWithHeaderCode(code);
    ASSERT_EQ(ret2, ret);

    code = HttpHeaderCode::HTTP_HEADER_COLON_METHOD;
    ret = HttpCommomHeaders::getPointerWithHeaderCode(code);
    ASSERT_STREQ(ret, ":Method");

    code = HttpHeaderCode::HTTP_HEADER_NONE;
    ret = HttpCommomHeaders::getPointerWithHeaderCode(code);
    ASSERT_STREQ(ret, "");

    code = HttpHeaderCode::HTTP_HEADER_OTHER;
    ret = HttpCommomHeaders::getPointerWithHeaderCode(code);
    ASSERT_STREQ(ret, "");

}

}//http_test