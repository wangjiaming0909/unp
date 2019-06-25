#include <iostream>
#include "http/http_parser/http_parser.h"
#include "boost/range.hpp"
#include <vector>
#include "http/http_parser/URLParser.h"
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
}