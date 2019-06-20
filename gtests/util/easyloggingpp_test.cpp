#include <gtest/gtest.h>
#include "server/util/easylogging++.h"
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

TEST(test_easylogging, test_it_in_multi_thread){

    // std::string *str = new std::string("123123");


    // std::thread thread1(thread_routine, std::ref(*str));
    // std::thread thread2(thread_routine, std::ref(*str));

    // main_routine(str);
    // thread1.join();
    // thread2.join();
}
