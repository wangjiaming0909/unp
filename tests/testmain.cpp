#define BOOST_TEST_MODULE boost_test_macro_overview
#include <boost/test/included/unit_test.hpp>
#include "../server/config/ServerConfig.h"
using namespace boost;
using namespace config;

BOOST_AUTO_TEST_CASE(test_read_config_file){
    ServerConfig config;
    BOOST_TEST(config.getReadConfigFileStatus() == true);
}

