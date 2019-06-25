
#include "string_tests.h"
#include "util/XString.h"
#include <cstdlib>
using namespace TEST;


void XSTRING_TEST::string_empty_string(){
    // CONSOLE_LOG(__func__);
    util::string s;
    assert(s.empty());
    assert(s.size() == 0);
    assert(s.capacity() == 64 + 64/2);
    ASSERT_OK;
}

void XSTRING_TEST::string_initialize_from_char_star(){
    // CONSOLE_LOG(__func__);
    util::string s = "123";
    assert(!s.empty());
    assert(s.size() == 3);
    assert(s.capacity() == 3 + 3/2);
    const char* ptr = s.ptr();
    assert(*ptr == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    ASSERT_OK;
}

void XSTRING_TEST::string_copy_constructor(){
    // CONSOLE_LOG(__func__);
    util::string s = "123";
    util::string s2 = s;
    assert(s2.ptr() != s.ptr());//new string should not be in the same memory
    assert(s2.size() == s.size());
    assert(s2.capacity() == s.capacity());
    const char* p1 = s.ptr();
    const char* p2 = s2.ptr();
    assert(*p1 == *p2);
    assert(*(p1+1) == *(p2+1));
    assert(*(p1+2) == *(p2+2));
    ASSERT_OK;
}

void XSTRING_TEST::string_initialize_with_size(){
    util::string s(10);
    assert(s.size() == 0);
    const char* ptr = s.ptr();
    assert(*ptr == '\0');
    assert(s.capacity() == 10 + 10/2);
    ASSERT_OK;
}


void XSTRING_TEST::string_initialize_with_char_and_size(){
    const char* ptr = "1234";
    util::string s(ptr + 1, 3);
    assert(s.size() == 3);
    assert(s.capacity() == 3 + 3/2);
    const char* p = s.ptr();
    assert(*p == '2');
    assert(*(p+1) == '3');
    assert(*(p+2) == '4');
    ASSERT_OK;
}

void XSTRING_TEST::string_initialize_with_empty_char()
{
    const char* ptr = "";
    util::string s = ptr;
    assert(s.size() == 0);
    assert(s.capacity() > 0);
    const char* p = s.ptr();
    assert(*p == '\0');
    ASSERT_OK;
}

void XSTRING_TEST::string_initialize_from_unsigned_char_star(){
    // const char* signed_ptr = "1234";
    // const unsigned char* signed_ptr = static_cast<unsigned char*>(signed_ptr);
    // util::string s(ptr);
    // assert(s.size() == 4);
    // assert(s.capacity() == 4 + 4/2);
    // const char* ptr = s.ptr();
    // assert(*ptr == '1');
    // assert(*(ptr+1) == '2');
    // assert(*(ptr+2) == '3');
    // assert(*(ptr+3) == '4');
}

void XSTRING_TEST::string_initialize_from_std_string(){
    std::string s = "1234";
    util::string us = s;
    assert(us.size() == 4);
    assert(us.capacity() == 4 + 4/2);
    const char* ptr = us.ptr();
    assert(*(ptr) == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    assert(*(ptr+3) == '4');
    ASSERT_OK;
}

void XSTRING_TEST::string_append_char_star_need_to_reallocate_memory(){
    util::string s = "1234";//size == 4, capacity == 6
    util::string& tmp = s.append("1234");//need to reallocate memory
    assert(s.size() == 8);
    assert(s.capacity() == 8 + 8/2);
    const char* ptr = s.ptr();
    assert(*(ptr) == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    assert(*(ptr+3) == '4');
    assert(*(ptr+4) == '1');
    assert(*(ptr+5) == '2');
    assert(*(ptr+6) == '3');
    assert(*(ptr+7) == '4');
    assert(&tmp == &s);//return value is the same as the original object
    ASSERT_OK;
}

void XSTRING_TEST::string_append_char_star_need_to_reallocate_memory_again(){
    util::string s = "1234";//size == 4, capacity == 6
    util::string& tmp = s.append("1234");//need to reallocate memory
    assert(s.size() == 8);
    assert(s.capacity() == 8 + 8/2);
    const char* ptr = s.ptr();
    assert(*(ptr) == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    assert(*(ptr+3) == '4');
    assert(*(ptr+4) == '1');
    assert(*(ptr+5) == '2');
    assert(*(ptr+6) == '3');
    assert(*(ptr+7) == '4');
    assert(&tmp == &s);//return value is the same as the original object
    ASSERT_OK;
}

void XSTRING_TEST::string_append_char_star_no_need_to_reallocate_memory()
{
    util::string str = "1234";
    str.append("12");
    assert(str.size() == 6);
    assert(str.capacity() == 6);
    const char* char_ptr = str.ptr();
    assert(*(char_ptr) == '1');
    assert(*(char_ptr+1) == '2');
    assert(*(char_ptr+2) == '3');
    assert(*(char_ptr+3) == '4');
    assert(*(char_ptr+4) == '1');
    assert(*(char_ptr+5) == '2');
    ASSERT_OK;
}

void XSTRING_TEST::string_append_std_string(){
    util::string s = "1234";
    util::string& tmp = s.append(std::string("1234"));
    assert(s.size() == 8);
    assert(s.capacity() == 8 + 8/2);
    const char* ptr = s.ptr();
    assert(*(ptr) == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    assert(*(ptr+3) == '4');
    assert(*(ptr+4) == '1');
    assert(*(ptr+5) == '2');
    assert(*(ptr+6) == '3');
    assert(*(ptr+7) == '4');
    assert(&tmp == &s);//return value is the same as the original object
    ASSERT_OK;
}

void XSTRING_TEST::string_append_char_star_len()
{
    util::string s = "1234";
    util::string& tmp = s.append("12345", 4);
    assert(s.size() == 8);
    assert(s.capacity() == 8 + 8/2);
    const char* ptr = s.ptr();
    assert(*(ptr) == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    assert(*(ptr+3) == '4');
    assert(*(ptr+4) == '1');
    assert(*(ptr+5) == '2');
    assert(*(ptr+6) == '3');
    assert(*(ptr+7) == '4');
    assert(&tmp == &s);//return value is the same as the original object
    ASSERT_OK;
}

void XSTRING_TEST::string_begin_and_end()
{
    util::string s = "1234";
    const char* ptr = s.ptr();
    const char* begin = s.begin();
    const char* end = s.end();
    assert(ptr == begin);
    assert(*begin == '1');
    assert(*end == '4');
    ASSERT_OK;
}

void XSTRING_TEST::string_operator_braket()
{
    util::string s = "1234";
    assert(s[0] == '1');
    assert(s[1] == '2');
    assert(s[2] == '3');
    assert(s[3] == '4');
    ASSERT_OK;
}

void XSTRING_TEST::string_operator_equal()
{
    util::string s = "1234";
    util::string s2 = "1234";
    util::string s3 = "1334";
    assert(s == s);
    assert(s == s2);
    assert(s != s3);
    ASSERT_OK;
}

void XSTRING_TEST::string_test_one_very_strange_issue()
{
    util::string str;
    str = "123";
    str.append("456");
    assert(str.size() == 6);
    const char* ptr = str.ptr();
    assert(*(ptr) == '1');
    assert(*(ptr+1) == '2');
    assert(*(ptr+2) == '3');
    assert(*(ptr+3) == '4');
    assert(*(ptr+4) == '5');
    assert(*(ptr+5) == '6');
    ASSERT_OK;
}

void XSTRING_TEST::string_to_c_str()
{
    util::string str = "1234";
//    const char* p = str.c_str();
    int value = int(str);
    assert( value == 1234);
    ASSERT_OK;
}




