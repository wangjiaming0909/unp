#include "http/http_parser/http_parser.h"
#include "boost/range.hpp"
#include "http/http_parser/URLParser.h"
#include "util/easylogging++.h"
#include "reactor/buffer.h"

#include <iostream>
#include <vector>
#include <gtest/gtest.h>

using namespace std;

namespace http_parser_test
{

class NotDefaultConstructableClass
{
public:
    NotDefaultConstructableClass(int){}
};

class NOTDestructable
{
public:
    ~NOTDestructable() = delete;
};


void test_traits()
{
    std::vector<string> vstr{};
    using it_t = std::vector<string>::iterator;
    it_t begin = vstr.begin();
    cout << begin.base();
    //when construct vector, it didn't check if T has default constructor
    std::vector<NotDefaultConstructableClass> v{};
    // v.resize(12);

    cout << std::is_trivially_default_constructible<NotDefaultConstructableClass>::value << endl;
    // using const_char = std::add_const<char*>::type;
    using const_char = const char*;
    const_char c = "asd";
    cout << c << endl;

    cout << typeid(std::remove_const_t<const_char>).name() << endl;

    cout << std::is_destructible<NotDefaultConstructableClass>::value << endl;
    cout << std::is_destructible<NOTDestructable>::value << endl;
    cout << std::is_destructible<void>::value << endl;
    //为什么const char*也是destructible的???
    cout << std::is_destructible<const_char>::value << endl;

    cout << "------------------------------------------" << endl;
    const char* cc = "asdasd";
    // using vector_it_t = std::vector<char>::iterator;

    // vector_it_t begin_it = c;


    std::string str = cc;
    // std::string::const_iterator beg = str.cbegin();

    // std::find(beg, str.end(), "/");
    auto index = str.find('s');
    cout << index << endl;
    index = str.find("asd");
    cout << index << endl;
}


void test()
{
#ifdef __gnu_linux__
    struct http_parser_url url;
    const char *site = "http://www.github.com/wangjiaming0909";
    http_parser_parse_url(site, strlen(site), 0, &url);
#endif //__gnu_linux__

    using it_traits = std::iterator_traits<const char*>;
    it_traits::pointer p = "asd";
    it_traits::difference_type difference = 1;
    cout << typeid(it_traits::iterator_category).name() << endl;
    cout << typeid(it_traits::reference).name() << endl;
    cout << typeid(it_traits::value_type).name() << endl;
    cout << difference << endl;
    cout << p << endl;

    const char* ppp = "abcdefg";
    using it_t = const char*;
    // using value_t = std::iterator_traits<const char*>::value_type;
    it_t begin = ppp + 1;
    it_t end = ppp + 7;
    // value_t value = *ppp;
    auto index = std::find(begin, end, 'g');
    if(index == nullptr)
    {
        // cout << "index is null" << endl;
    }
    cout << "index: " << index << endl;


    // boost::range_iterator<char> string_piece;
    // boost::range_result_iterator<char>
    cout << "testing http_parser" << endl;
}

TEST(http_parser, test_URLParser)
{
    http::URLParser parser{"https://a.com/c/d"};

    cout << parser.scheme() << endl;
    cout << parser.host() << endl;
    cout << parser.path() << endl;
    cout << parser.port() << endl;

    assert(parser.scheme().size() == 5);
    string_piece::const_string_piece scheme = "https";
    assert(parser.scheme() == scheme);

    assert(parser.host().size() == 5);
    string_piece::const_string_piece host = "a.com";
    assert(parser.host() == host);

    string_piece::const_string_piece path = "/c/d";
    assert(parser.path().size() == 4);
    assert(parser.path() == path);

    assert(parser.port() == 0);
}

#define PRINT(i) cout << (i) << endl; return 0

int onMessageBeginCB(http_parser* parser) 
{ 
    PRINT(http_method_str(http_method(parser->method)));
    PRINT(http_status_str(http_status(parser->state)));
    PRINT(http_errno_name(http_errno(parser->http_errno)));
    PRINT(1); 
}
int onUrlCB(http_parser* parser, const char* buf, size_t len) {PRINT(2);  }
int onHeaderFieldCB(http_parser* parser, const char* buf, size_t len) { PRINT(3); }
int onHeaderValueCB(http_parser* parser, const char* buf, size_t len) {PRINT(4);}
int onHeadersCompleteCB(http_parser* parser) {PRINT(5); }
int onBodyCB(http_parser* parser, const char* buf, size_t len) {PRINT(6); }
int onMessageCompleteCB(http_parser* parser) { PRINT(7); }
int onReasonCB(http_parser* parser, const char* buf, size_t len){PRINT(8); }
int onChunkHeaderCB(http_parser* parser){PRINT(9);}
int onChunkCompleteCB(http_parser* parser){PRINT(10);}



TEST(HttpParser, normal)
{
    // GTEST_SKIP();
    http_parser parser;
    http_parser_init(&parser, http_parser_type::HTTP_REQUEST);

    http_parser_settings settings;
    http_parser_settings_init(&settings);

    settings.on_message_begin = onMessageBeginCB;
    settings.on_url = onUrlCB;
    settings.on_header_field = onHeaderFieldCB;
    settings.on_header_value = onHeaderValueCB;
    settings.on_headers_complete = onHeadersCompleteCB;
    settings.on_body = onBodyCB;
    settings.on_message_complete = onMessageCompleteCB;
    // settings.on_reason = onReasonCB;
    settings.on_chunk_header = onChunkHeaderCB;
    settings.on_chunk_complete = onChunkCompleteCB;


    const char* requestStr = "GET http://www.herongyang.com/Service/Hello_REST.php?Message=Hello%20from%20client. HTTP/1.1\n\
Accept-Encoding: gzip,deflate\n\
Host: www.herongyang.com\n\
Connection: Keep-Alive\n\
User-Agent: Apache-HttpClient/4.1.1 (java 1.5)\n\n";

    http_parser_execute(&parser, &settings, requestStr, strlen(requestStr));
    LOG(INFO) << (http_errno_name(http_errno(parser.http_errno)));
    ASSERT_EQ(parser.http_errno, 0);

    requestStr = "GET http://www.baidu.com HTTP/1.1\n";
    http_parser_execute(&parser, &settings, requestStr, strlen(requestStr));
    LOG(INFO) << (http_errno_name(http_errno(parser.http_errno)));
    ASSERT_EQ(parser.http_errno, 0);

    requestStr = "GET http://www.baidu.com HTT";
    http_parser_execute(&parser, &settings, requestStr, strlen(requestStr));
    LOG(INFO) << (http_errno_name(http_errno(parser.http_errno)));
    ASSERT_EQ(parser.http_errno, http_errno::HPE_INVALID_HEADER_TOKEN);

    requestStr = "GET http://www.baidu.com HTTP/1.1\n\nGET http://www.baidu.com HTTP/1.1\n";
    http_parser_execute(&parser, &settings, requestStr, strlen(requestStr));
    LOG(INFO) << (http_errno_name(http_errno(parser.http_errno)));
    ASSERT_EQ(parser.http_errno, http_errno::HPE_INVALID_HEADER_TOKEN);
    cout << "------------------------------------------------------------------------------" << endl;

    const char* responseStr = "HTTP/1.1 200 OK\n\
Cache-Control: private\n\
Connection: Keep-Alive\n\
Content-Length: 68\n\
Content-Encoding: gzip\n\
Content-Type: text/html;charset=utf-8\n\
Date: Tue, 30 Jul 2019 04:52:20 GMT\n\
Server: BWS/1.1\n\
Set-Cookie: delPer=0; path=/; domain=.baidu.com\n\
Set-Cookie: BD_CK_SAM=1;path=/\n\
Set-Cookie: PSINO=7; domain=.baidu.com; path=/\n\
Set-Cookie: BDSVRTM=16; path=/\n\
Set-Cookie: H_PS_PSSID=26524_1422_21102_29522_29521_28519_29099_29568_28839_29221_26350_22160; path=/; domain=.baidu.com\n\n\
<html>\n\
<body>\n\
<h1>first tite</h1>\n\
<p>first para</p>\n\
</body>\n\
</html>\n";

    http_parser responseParser;
    http_parser_init(&responseParser, http_parser_type::HTTP_RESPONSE);

    http_parser_execute(&responseParser, &settings, responseStr, strlen(responseStr));
    LOG(INFO) << (http_errno_name(http_errno(parser.http_errno)));
    ASSERT_EQ(responseParser.http_errno, 0);
    ASSERT_EQ(responseParser.status_code, HTTP_STATUS_OK);
    // ASSERT_EQ(responseParser.content_length, 1);
    // ASSERT_EQ(responseParser.method, HTTP_GET);
    cout << "---------------------------------------" << endl;
}


TEST(HttpParser, ParserUsingBuffer){
    using namespace reactor;
    http_parser_settings settings;
    http_parser parser;

    buffer messageBuf{};

    const char* method = "GET";
    const char* url = "http://www.baidu.com";
    const char* httpVersion = "HTTP/1.1";

    // messageBuf.append()

}

}