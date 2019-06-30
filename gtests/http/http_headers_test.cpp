#include <gtest/gtest.h>
#include "http/HttpHeaders.h"

using namespace http;

namespace http_test
{

void _()
{
    (void)wordlist;
}

void assertHeadersSize(const HttpHeaders& headers, size_t size)
{
    ASSERT_EQ(headers.size(), size);
}

void assertCodesSize(const HttpHeaders& headers, size_t size, int64_t capacity)
{
    ASSERT_EQ(headers.getCodes().size(), size);
    if(capacity >= 0)
    {
        ASSERT_EQ(headers.getCodes().capacity(), capacity);
    }
}

void assertHeaderNameSize(const HttpHeaders& headers, size_t size, int64_t capacity)
{
    ASSERT_EQ(headers.getHeaderNames().size(), size);
    if(capacity >= 0)
    {
        ASSERT_EQ(headers.getHeaderNames().capacity(), capacity);
    }
}

void assertHeaderValueSize(const HttpHeaders& headers, size_t size, int64_t capacity)
{
    ASSERT_EQ(headers.getHeaderValues().size(), size);
    if(capacity >= 0)
    {
        ASSERT_EQ(headers.getHeaderValues().capacity(), capacity);
    }
}

TEST(http_headers, constructor)
{
    HttpHeaders headers{};

    assertHeadersSize(headers, 0);
    assertCodesSize(headers, 0, HttpHeaders::getInitVectorReserveSize());
    assertHeaderNameSize(headers, 0, HttpHeaders::getInitVectorReserveSize());
    assertHeaderValueSize(headers, 0, HttpHeaders::getInitVectorReserveSize());
    ASSERT_EQ(headers.getCodesDeleted(), 0);
}

TEST(http_headers, copy_constructor_without_headers)
{
    HttpHeaders headers1{};
    HttpHeaders headers2 = headers1;

    assertHeadersSize(headers1, 0);
    assertCodesSize(headers1, 0, HttpHeaders::getInitVectorReserveSize());
    assertHeaderNameSize(headers1, 0, HttpHeaders::getInitVectorReserveSize());
    assertHeaderValueSize(headers1, 0, HttpHeaders::getInitVectorReserveSize());
    ASSERT_EQ(headers1.getCodesDeleted(), 0);

    assertHeadersSize(headers2, 0);
    assertCodesSize(headers2, 0, 0);
    assertHeaderNameSize(headers2, 0, 0);
    assertHeaderValueSize(headers2, 0, 0);
    ASSERT_EQ(headers2.getCodesDeleted(), 0);
}

TEST(http_headers, copy_constructor_with_headers)
{
    HttpHeaders headers1{};
    string_piece::const_string_piece headerName1 = "Cache-Control";
    string_piece::const_string_piece headerValue1 = "private";
    headers1.add(headerName1, headerValue1);
    HttpHeaders headers2 = headers1;

    assertHeadersSize(headers2, 1);
    assertCodesSize(headers2, 1, -1);
    assertHeaderNameSize(headers2, 1, -1);
    assertHeaderValueSize(headers2, 1, -1);
    ASSERT_EQ(headers2.getCodesDeleted(), 0);

    ASSERT_EQ(headers2.getCodes().back(), HttpHeaderCode::HTTP_HEADER_CACHE_CONTROL);
    ASSERT_EQ(memcmp(headers2.getHeaderNames().back(), headerName1.cbegin(), headerName1.size()), 0);
    ASSERT_EQ(headers2.getHeaderNames().back(), HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode::HTTP_HEADER_CACHE_CONTROL));
    ASSERT_EQ(headers2.getHeaderValues().back(), std::string(headerValue1.cbegin(), headerValue1.size()));
}

TEST(http_headers, add_using_string_piece)
{
    //Cache-Control: private
    string_piece::const_string_piece headerName1 = "Cache-Control";
    string_piece::const_string_piece headerValue1 = "private";

    HttpHeaders headers{};
    headers.add(headerName1, headerValue1);

    assertHeadersSize(headers, 1);
    assertCodesSize(headers, 1, -1);
    assertHeaderNameSize(headers, 1, -1);
    assertHeaderValueSize(headers, 1, -1);
    ASSERT_EQ(headers.getCodesDeleted(), 0);

    ASSERT_EQ(headers.getCodes().back(), HttpHeaderCode::HTTP_HEADER_CACHE_CONTROL);
    ASSERT_EQ(memcmp(headers.getHeaderNames().back(), headerName1.cbegin(), headerName1.size()), 0);
    ASSERT_EQ(headers.getHeaderNames().back(), HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode::HTTP_HEADER_CACHE_CONTROL));
    ASSERT_EQ(headers.getHeaderValues().back(), std::string(headerValue1.cbegin(), headerValue1.size()));

    // Content-Type: text/html;charset=utf-8
    string_piece::const_string_piece headerName2 = "Content-Encoding";
    string_piece::const_string_piece headerValue2 = "gzip";

    headers.add(headerName2, headerValue2);

    assertHeadersSize(headers, 2);
    assertCodesSize(headers, 2, -1);
    assertHeaderNameSize(headers, 2, -1);
    assertHeaderValueSize(headers, 2, -1);
    ASSERT_EQ(headers.getCodesDeleted(), 0);

    ASSERT_EQ(headers.getCodes().back(), HttpHeaderCode::HTTP_HEADER_CONTENT_ENCODING);
    ASSERT_EQ(memcmp(headers.getHeaderNames().back(), headerName2.cbegin(), headerName2.size()), 0);
    ASSERT_EQ(headers.getHeaderNames().back(), HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode::HTTP_HEADER_CONTENT_ENCODING));
    ASSERT_EQ(headers.getHeaderValues().back(), std::string(headerValue2.cbegin(), headerValue2.size()));

    //duplicated entities of headers
    headers.add(headerName2, headerValue2);
    assertHeadersSize(headers, 3);
    assertCodesSize(headers, 3, -1);
    assertHeaderNameSize(headers, 3, -1);
    assertHeaderValueSize(headers, 3, -1);
    ASSERT_EQ(headers.getCodesDeleted(), 0);

    ASSERT_EQ(headers.getCodes().back(), HttpHeaderCode::HTTP_HEADER_CONTENT_ENCODING);
    ASSERT_EQ(memcmp(headers.getHeaderNames().back(), headerName2.cbegin(), headerName2.size()), 0);
    ASSERT_EQ(headers.getHeaderNames().back(), HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode::HTTP_HEADER_CONTENT_ENCODING));
    ASSERT_EQ(headers.getHeaderValues().back(), std::string(headerValue2.cbegin(), headerValue2.size()));

    //some headers not included in commom headers maybe
    string_piece::const_string_piece headerName3 = "AAAAA";
    string_piece::const_string_piece headerValue3 = "BBBBB";

    headers.add(headerName3, headerValue3);
    assertHeadersSize(headers, 4);
    assertCodesSize(headers, 4, -1);
    assertHeaderNameSize(headers, 4, -1);
    assertHeaderValueSize(headers, 4, -1);
    ASSERT_EQ(headers.getCodesDeleted(), 0);

    ASSERT_EQ(headers.getCodes().back(), HttpHeaderCode::HTTP_HEADER_OTHER);
    ASSERT_EQ(memcmp(headers.getHeaderNames().back(), headerName3.cbegin(), headerName3.size()), 0);
    ASSERT_EQ(headers.getHeaderValues().back(), std::string(headerValue3.cbegin(), headerValue3.size()));
}

TEST(http_headers, remove)
{
    string_piece::const_string_piece headerName1 = "Cache-Control";
    string_piece::const_string_piece headerValue1 = "private";
    string_piece::const_string_piece headerName2 = "Content-Encoding";
    string_piece::const_string_piece headerValue2 = "gzip";

    HttpHeaders header{};
    header.add(headerName1, headerValue1);

    auto removed = header.remove(headerName1);
    ASSERT_EQ(removed, true);
    ASSERT_EQ(header.size(), 0);

    header.add(headerName1, headerValue1);
    header.add(headerName1, headerValue1);
    header.add(headerName2, headerValue2);
    removed = header.remove(headerName1);
    ASSERT_EQ(removed, true);
    ASSERT_EQ(header.size(), 1);
    ASSERT_EQ(header.getCodesDeleted(), 3);
}
}