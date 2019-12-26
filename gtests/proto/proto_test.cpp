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
