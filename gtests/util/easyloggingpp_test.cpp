#include <gtest/gtest.h>
#include "server/util/easylogging++.h"


// using namespace util;


TEST(test_easylogging, test_it_in_multi_thread){
    LOG(INFO) << 123;
}
