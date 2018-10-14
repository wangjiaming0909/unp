// #define BOOST_TEST_MODULE boost_test_macro_overview
// #include <boost/test/included/unit_test.hpp>

// using namespace boost;
#include "tests_macros.h"
#include "string_tests.h"
#include "fileutil_test.h"
#include "server_conf_test.h"
#include "server_test.h"
#include "pmutex_test.h"
#include "pthread_test.h"
#include "../server/util/easylogging++.h"

void setupLogger();

INITIALIZE_EASYLOGGINGPP
int main()
{
    using namespace TEST;
    setupLogger();
    LOG(INFO) << "test started----";
    XSTRING_TEST::test();
    FILEUTIL_TEST::test();
    SERVER_CONFIG_TEST::test();
    SERVER_TEST::test();
    PMUTEX_TEST::test();
    UNP_PTHREAD_TEST::test();
    LOG(INFO) << "test ended----";
    return 0;
}

void setupLogger(){
    using namespace el;
    Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.set(Level::Global, ConfigurationType::Enabled, "true");
    defaultConf.set(Level::Global, ConfigurationType::Format, "%datetime, %thread, %thread_name, %level, %file, %line, %func, %msg");
    defaultConf.set(Level::Global, ConfigurationType::PerformanceTracking, "false");
    defaultConf.set(Level::Global, ConfigurationType::ToStandardOutput, "false");
    defaultConf.set(Level::Global, ConfigurationType::ToFile, "true");
    defaultConf.set(Level::Global, ConfigurationType::Filename, "/tmp/unp_test.log");
    defaultConf.set(Level::Global, ConfigurationType::LogFlushThreshold, "100");
    defaultConf.set(Level::Global, ConfigurationType::MaxLogFileSize, "2097152");

    Loggers::reconfigureAllLoggers(defaultConf);
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
