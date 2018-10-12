// #define BOOST_TEST_MODULE boost_test_macro_overview
// #include <boost/test/included/unit_test.hpp>

// using namespace boost;
#include "tests_macros.h"
#include "string_tests.h"
#include "fileutil_test.h"
#include "server_conf_test.h"
#include "server_test.h"
#include "pmutex_test.h"
#include "../server/util/easylogging++.h"

INITIALIZE_EASYLOGGINGPP
int main()
{
    using namespace TEST;
    XSTRING_TEST::test();
    FILEUTIL_TEST::test();
    SERVER_CONFIG_TEST::test();
    SERVER_TEST::test();
    PMUTEX_TEST::test();


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
