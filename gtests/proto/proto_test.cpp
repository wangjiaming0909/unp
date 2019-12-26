#include <cstdlib>
#include <gtest/gtest.h>
#include "proto/mess.pb.h"
#include <iostream>

const char* url = "https://www.github.com/wangjiaming0909";

void setMess(httpmessage::Mess& mes )
{
    int id = 1;
    mes.set_id(id);
    mes.set_url(url);
    mes.set_command(httpmessage::Mess::Command::Mess_Command_RESUME);
}

TEST(proto, normal)
{
    httpmessage::Mess mes{};
    setMess(mes);
    char* arr = (char*)::calloc(1024, 1);
    mes.SerializeToArray(arr, 1024);
    auto sizeOfMes = mes.ByteSizeLong();

    httpmessage::Mess mes2{};
    mes2.ParseFromArray(arr, 1024);
    ASSERT_EQ(mes2.has_id(), true);
    ASSERT_EQ(mes2.id(), 1);
    ASSERT_EQ(mes2.command(), httpmessage::Mess::Command::Mess_Command_RESUME);
    ASSERT_FALSE(mes2.command() == httpmessage::Mess::Command::Mess_Command_PAUSE);
    ASSERT_STREQ(mes2.url().c_str(), url);
    mes2.set_url("\0");
    std::cout << "ByteSizeLong: " << mes2.ByteSizeLong() << std::endl;
    std::cout << "SpaceUsedLong: " << mes2.SpaceUsedLong() << std::endl;
    ASSERT_TRUE(mes2.IsInitialized());

    httpmessage::Mess mes3{};
    ASSERT_FALSE(mes3.IsInitialized());
    mes3.set_id(1);
    ASSERT_FALSE(mes3.IsInitialized());

    httpmessage::Mess mes4{};

    mes4.ParsePartialFromArray(arr, 1024);//using partial parse, but the data in arr is enough
    ASSERT_TRUE(mes4.IsInitialized());
    std::cout << "sizeOfMes: " << sizeOfMes << std::endl;
    std::cout << "sizeOfMes4: " << mes4.ByteSizeLong() << std::endl;
    ASSERT_EQ(mes4.ByteSizeLong(), sizeOfMes);


    httpmessage::Mess mes5{};
    mes5.ParsePartialFromArray(arr, 2);//use partial parse, but the data size we give is not enough(size is 44)
    std::cout << "sizeOfMes5: " << mes5.ByteSizeLong() << std::endl;
    //std::cout << "url: " << mes5.url() << std::endl;
    std::cout << "url size: " << mes5.url().size() << std::endl;
    std::cout << "id: " << mes5.id() << std::endl;
    std::cout << "command: " << mes5.command() << std::endl;
    ASSERT_FALSE(mes5.IsInitialized());
    httpmessage::Mess mes6{};
    mes6.ParsePartialFromArray(arr+2, 42);
    mes5.MergeFrom(mes6);
    std::cout << "after parsed all..." << std::endl;
    std::cout << "sizeOfMes5: " << mes5.ByteSizeLong() << std::endl;
    std::cout << "url: " << mes5.url() << std::endl;
    std::cout << "url size: " << mes5.url().size() << std::endl;
    std::cout << "id: " << mes5.id() << std::endl;
    std::cout << "command: " << mes5.command() << std::endl;
    ASSERT_TRUE(mes5.IsInitialized());

    ::free(arr);
    arr = nullptr;
}

TEST(proto, merge_from)
{
    httpmessage::Mess m1{};
    m1.set_id(1);
    m1.set_command(httpmessage::Mess_Command::Mess_Command_DOWNLOAD);
    m1.set_url("https://www.baidu.com");
    auto m1Size = m1.ByteSizeLong();
    ASSERT_TRUE(m1Size < 512);

    httpmessage::Mess m2{};
    m2.CopyFrom(m1);
    m2.set_command(httpmessage::Mess_Command::Mess_Command_PAUSE);
    auto m2Size = m2.ByteSizeLong();

    char* d = (char*)::calloc(1024, 1);

    m1.SerializeToArray(d, 1024);
    std::cout << d << std::endl;

    m2.SerializeToArray(d + m1Size, 1024 - m1Size);
    std::cout << d << std::endl;

    httpmessage::Mess m3{};

    auto ret = m3.ParsePartialFromArray(d, m2Size + m1Size);
    ASSERT_TRUE(ret);
    ASSERT_EQ(m1.url(), m3.url());
    ASSERT_EQ(m1.id(), m3.id());
    ASSERT_EQ(m2.command(), m3.command());

    m3.Clear();
    m3.ParseFromArray(d, m1Size);
    ASSERT_EQ(m1.url(), m3.url());
    ASSERT_EQ(m1.id(), m3.id());
    ASSERT_EQ(m1.command(), m3.command());
    m3.Clear();
    m3.ParseFromArray(d + m1Size, 1024 - m1Size);
    ASSERT_EQ(m2.url(), m3.url());
    ASSERT_EQ(m2.id(), m3.id());
    ASSERT_EQ(m2.command(), m3.command());

    free(d);
    d = nullptr;
}

TEST(proto, parse_from_array)
{
    httpmessage::Mess m1{};
    m1.set_id(1);
    m1.set_command(httpmessage::Mess_Command::Mess_Command_DOWNLOAD);
    m1.set_url("https://github.com/wangjiaming0909/unp");
    auto m1Size = m1.ByteSizeLong();

    httpmessage::Mess m2{m1};
    m2.set_command(httpmessage::Mess_Command::Mess_Command_PAUSE);

    char* d = (char*)::calloc(1024, 1);
    m1.SerializeToArray(d, 1024);

    m2.SerializeToArray(d+m1Size, 1024 - m1Size);

    free(d);
    d = nullptr;
}