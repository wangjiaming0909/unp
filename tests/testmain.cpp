// #define BOOST_TEST_MODULE boost_test_macro_overview
// #include <boost/test/included/unit_test.hpp>
#include "../server/util/XString.h"
#include <iostream>
#include <cassert>
#include <iomanip>
// using namespace boost;
#ifndef CONSOLE_LOG 
#define CONSOLE_LOG(message) std::cout << message
#endif

#ifndef ASSERT_OK
#define ASSERT_OK std::cout << std::left << "--" << std::setw(50) << __func__ << std::right << std::setw(4) << "OK" << std::endl
#endif

void test_string_empty_string(){
    // CONSOLE_LOG(__func__);
    util::string s;
    assert(s.empty());
    assert(s.size() == 0);
    assert(s.capacity() == 64 + 64/2);
    ASSERT_OK;
}

void test_string_initialize_from_char_star(){
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

void test_string_copy_constructor(){
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

void string_initialize_with_size(){
    util::string s(10);

    ASSERT_OK;
}


int main()
{
    CONSOLE_LOG("test XString");
    test_string_empty_string();
    test_string_initialize_from_char_star();
    test_string_copy_constructor();
    return 0;
}

// BOOST_AUTO_TEST_CASE(test_read_config_file){
//     ServerConfig config;
//     BOOST_TEST(config.getReadConfigFileStatus() == true);
// }

// BOOST_AUTO_TEST_CASE(test_XString_Constructor_with_no_parameters){
//     util::string s;
//     BOOST_TEST(s.empty());//the new string should be empty
//     BOOST_TEST(s.capacity() == (64 + 64/2));//the new string's capacity should be this value
// }

// BOOST_AUTO_TEST_CASE(test_XString_copy_structor){
//     const char* ptr = "123";
// }

// BOOST_AUTO_TEST_CASE(test_XString_char_ptr_constructor){
//    const char* ptr = "123";
//    util::string s = ptr;//constructor with const char* parameter
//    BOOST_TEST(s.ptr() != ptr);//the memory should be reallocated
//    const char* p1 = s.ptr();
//    BOOST_TEST(*p1 == '1');//exam the content in the string
//    BOOST_TEST(*(p1+1) == '2');
//    BOOST_TEST(*(p1+2) == '3');
// }
