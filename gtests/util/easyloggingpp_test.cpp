#include <gtest/gtest.h>
#include "util/easylogging++.h"
#include <thread>


using namespace std::chrono_literals;

void thread_routine(const std::string& s)
{
    while(1)
    {
        std::this_thread::sleep_for(100ms);
        LOG(INFO) << s << "logging";
    }
}

void main_routine(std::string* str)
{
    std::this_thread::sleep_for(1s);
    delete str;
}
