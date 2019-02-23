#include <gtest/gtest.h>
#include "server/thread/message_queue.h"
#include <string>   
#include <poll.h>
#include <vector>

TEST(message_queue_test, test_constructor){
    thread::message_queue<std::string> mq{};
    ASSERT_TRUE(!mq.is_full());
    ASSERT_TRUE(mq.is_empty());
    int count = mq.current_message_count();
    ASSERT_EQ(count, 0);
}

TEST(message_queue_test, test_dequeue_and_enqueue){
    thread::message_queue<std::string> mq{};
    std::chrono::seconds _{0};

    std::string message1{"message1"};
    thread::data_block<std::string> data_b1{&message1, false};
    mq.enqueue_head(data_b1, _);
    int count = mq.current_message_count();
    ASSERT_EQ(count, 1);

    thread::data_block<std::string> data_b2{new std::string("message2"), true};
    mq.enqueue_tail(data_b2, _);
    count = mq.current_message_count();
    ASSERT_EQ(count, 2);

    thread::data_block<std::string> data_b3{};
    auto *p1 = &data_b3;//这个指针的引用未免太恶心了吧, 传进去还必须先获取其指针
    mq.dequeue_head(p1, _);
    ASSERT_EQ(data_b3.get(), &message1);// 地址是相同的， 所以data_b3所指的内存就是message1的内存地址
    mq.dequeue_tail(p1, _);
    ASSERT_EQ(data_b3.get(), data_b2.get());//message2同理
}