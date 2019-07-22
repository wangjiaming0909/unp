#include <gtest/gtest.h>
#include "util/unp_time.h"
#include <chrono>

class unp_time_test : public ::testing::Test{
protected:
    unp_time_test(){}
    ~unp_time_test()override{}
    void SetUp() override{ }
    void TearDown() override{}
};

TEST_F(unp_time_test, test_from_std_time_point){
    auto tp = std::chrono::system_clock::now();
    util::Time t{tp};
    auto micro_seconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();
    ASSERT_EQ(t.microSecondsSinceEpoch(), micro_seconds_since_epoch);
    ASSERT_EQ(1,1);
}

TEST_F(unp_time_test, test_to_std_time_point){
    auto t = util::Time::now();
    const auto & tp = t.to_chrono_time_point<std::chrono::microseconds>();
    ASSERT_EQ(t.microSecondsSinceEpoch(), tp.time_since_epoch().count());
}

TEST(duration_to_timeval, duration_to_timeval_us){
    using namespace std::chrono_literals;
    std::chrono::microseconds uss = 1us + 1s;
    timeval time_val = util::duration_to_timeval(uss);
    ASSERT_EQ(time_val.tv_sec, 1);
    ASSERT_EQ(time_val.tv_usec, 1);
}
TEST(duration_to_timeval, duration_to_timeval_ms){
    using namespace std::chrono_literals;
    std::chrono::milliseconds ms = 1ms + 1s;
    timeval time_val = util::duration_to_timeval(ms);
    ASSERT_EQ(time_val.tv_sec, 1);
    ASSERT_EQ(time_val.tv_usec, 1000);
}

TEST(timeval_to_duration, timeval_to_duration){
    timeval time_val;
    time_val.tv_sec = 1;
    time_val.tv_usec = 1000;
    auto s = util::timeval_to_duration<std::chrono::seconds>(time_val);
    ASSERT_EQ(s.count(), 1);
    auto us = util::timeval_to_duration<std::chrono::microseconds>(time_val);
    ASSERT_EQ(us.count(), 1000000 + 1000);
}
