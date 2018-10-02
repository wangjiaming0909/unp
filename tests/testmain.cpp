#define BOOST_TEST_MODULE boost_test_macro_overview
#include <boost/test/included/unit_test.hpp>
#include "../server/config/ServerConfig.h"
#include "../server/util/XString.h"
using namespace boost;
using namespace config;

// BOOST_AUTO_TEST_CASE(test_read_config_file){
//     ServerConfig config;
//     BOOST_TEST(config.getReadConfigFileStatus() == true);
// }

BOOST_AUTO_TEST_CASE(test_XString_Constructor_with_no_parameters){
    util::string s;
    BOOST_TEST(s.empty());
    BOOST_TEST(s.capacity() == (64 + 64/2));
}

BOOST_AUTO_TEST_CASE(test_XString_char_ptr_constructor){
   const char* ptr = "123";
   util::string s = ptr;
   BOOST_TEST(s.ptr() != ptr);
   const char* p1 = s.ptr();
//    BOOST_TEST()
}




