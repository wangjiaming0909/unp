#include "util/string_piece/string_piece.h"
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

namespace string_piece_test
{

TEST(string_piece, test_sub_string)
{
    std::string str = "1234567890";
    string_piece::const_string_piece sp{str};

    auto subString = sp.sub_string(0, 10);
    assert(subString.begin() == &*str.begin());
    assert(subString.end() == &*str.end());
    assert(subString.size() == str.length());
}

TEST(string_piece, test_sub_string2)
{
    const char* data = "123456789";
    char* ptr = static_cast<char*>(::calloc(10, 1));
    ::memcpy(ptr, data, 6);
    string_piece::mutable_string_piece str2{ptr, ptr + 5};

    auto subString = str2.sub_string(0, 5);
    assert(subString.begin() == ptr);
    assert(subString.end() == ptr + 5);

    ::free(ptr);
}

TEST(string_piece, test_find)
{
    string_piece::const_string_piece text = "https://github.com/wangjiaming0909";
    string_piece::const_string_piece pattern = "://";
    auto pos = text.find(pattern);
    cout << "pos: " << pos << endl;
    assert(pos == 5);
    string_piece::const_string_piece pattern1 = "https:";
    pos = text.find(pattern1);
    assert(pos == 0);
}

TEST(string_piece, copy)
{
    string_piece::const_string_piece url = "https://github.com/wangjiaming0909";
    auto data = url.copy();
    ASSERT_EQ(memcmp(data, url.cbegin(), url.size()), 0);
    ASSERT_EQ(strlen(data), url.size());
    free(data);

    const char* data3 = "https://github.com/wangjiaming0909";
    size_t len = strlen(data3);
    char *ptr = static_cast<char *>(::calloc(len + 1, 1));
    ::memcpy(ptr, data3, len);
    string_piece::mutable_string_piece url2{ptr, ptr + len};
    auto data2 = url2.copy();
    ASSERT_EQ(memcmp(data2, url2.cbegin(), len), 0);
    ASSERT_EQ(strlen(data2), url2.size());
    free(data2);
}

TEST(string_piece, CaseInsensitiveEqual)
{
    string_piece::const_string_piece url = "https";
    const string str = "HTTPS";
    const char* ptr = "Https";

    auto ret = string_piece::CaseInSensitiveEqual<string_piece::const_string_piece, const char*>()(ptr, url);
    ASSERT_EQ(ret, true);
    ret = string_piece::CaseInSensitiveEqual<const char*, string_piece::const_string_piece>()(ptr, url);
    ASSERT_EQ(ret, true);

    ret = string_piece::CaseInSensitiveEqual<string_piece::const_string_piece, const char*>()(url, ptr);
    ASSERT_EQ(ret, true);

    ret = string_piece::CaseInSensitiveEqual<std::string, const char*>()(ptr, str);
    ASSERT_EQ(ret, true);

    ret = string_piece::CaseInSensitiveEqual<std::string, string_piece::const_string_piece>()(url, str);
    ASSERT_EQ(ret, true);
}

TEST(string_piece, CaseInsensitiveEqual_member_function)
{
    const char* ptr = "HTTPS";
    auto* ptr2 = const_cast<char*>(ptr);
    const char* ptr3 = "Https";
    string_piece::const_string_piece url = "https";
    string_piece::mutable_string_piece url2 = const_cast<char*>(ptr3);

    auto ret = url.caseInsensitiveEqual(ptr);
    ASSERT_EQ(ret, true);

    ret = url.caseInsensitiveEqual(ptr2);
    ASSERT_EQ(ret, true);

    ret = url2.caseInsensitiveEqual(ptr);
    ASSERT_EQ(ret, true);

    ret = url2.caseInsensitiveEqual(ptr2);
    ASSERT_EQ(ret, true);
}

TEST(string_piece, size)
{
    using namespace string_piece;
    const_string_piece str1 = "abc";
    ASSERT_EQ(str1.size(), 3);
    str1 = "1";
    ASSERT_EQ(str1.size(), 1);
}

TEST(string_piece, cmp)
{
    using namespace string_piece;
    const_string_piece s1 = "abb";
    const_string_piece s2 = "abcd";
    auto b = s1 < s2;
    ASSERT_TRUE(b);
    s1 = "abc";
    b = s1 < s2;
    ASSERT_TRUE(b);
}

void test()
{
    string_piece::mutable_string_piece str{};
    const char* data = "123456789";
    char* ptr = static_cast<char*>(::calloc(10, 1));
    ::memcpy(ptr, data, 6);
    string_piece::mutable_string_piece str2{ptr, ptr + 5};

    auto index = str2.find('2');
    cout << "index: " << index << endl;
    index = str2.find('-');
    cout << "index: " << index << endl;

    std::string strstr = "123456";
    //mutable_string_piece do not have this constructor
    // string_piece::mutable_string_piece str3{strstr};

    string_piece::const_string_piece str4{strstr};
    string_piece::mutable_string_piece str5{ptr};

    ::free(ptr);

    // std::vector<int> v{1,2,3};
    // string_piece::Range<int *> int1{&*v.begin()};

    cout << 123 << endl;
}

}