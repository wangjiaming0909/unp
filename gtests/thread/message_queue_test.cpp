#include <gtest/gtest.h>
#include "server/thread/message_queue.h"
#include <string>   

TEST(message_queue_test, test_constructor){
    thread::message_queue<std::string> mq{};
    ASSERT_TRUE(!mq.is_full());
    ASSERT_TRUE(mq.is_empty());
    int count = mq.current_message_count();
    ASSERT_EQ(count, 0);
    std::string message1{"message1"};
    thread::data_block<std::string> data_b1{&message1, sizeof(message1), false};
    mq.enqueue_head(&data_b1, 0);
    count = mq.current_message_count();
    ASSERT_EQ(count, 1);
    std::string message2{"message2"};
    thread::data_block<std::string> data_b2{&message2, sizeof(message2), false};
    mq.enqueue_tail(&data_b2, 0);
    count = mq.current_message_count();
    ASSERT_EQ(count, 2);

    std::string message3{};
    thread::data_block<std::string> data_b3{&message3, sizeof(message3), false};
    auto *p1 = &data_b3;
    mq.dequeue_head(p1, 0);
    mq.dequeue_tail(p1, 0);
    ASSERT_EQ(message3, "message1");
    ASSERT_EQ(message3, "message2");
}