#include <gtest/gtest.h>

#include "proto/mess.pb.h"

const char* url = "https://www.github.com/wangjiaming0909";

void setMess(httpmessage::Mess& mes )
{
    int id = 1;
    mes.set_id(id);
    mes.set_url(url);
    mes.set_command(httpmessage::Mess::Command::Mess_Command_DOWNLOAD);
}

TEST(proto, normal)
{
    httpmessage::Mess mes{};
    setMess(mes);
    char* arr = (char*)::calloc(1024, 1);
    mes.SerializeToArray(arr, 1024);

    httpmessage::Mess mes2{};
    mes2.ParseFromArray(arr, 1024);
    ::free(arr);
    ASSERT_EQ(mes2.has_id(), true);
    ASSERT_EQ(mes2.id(), 1);
    ASSERT_EQ(mes2.command(), httpmessage::Mess::Command::Mess_Command_DOWNLOAD);
    ASSERT_FALSE(mes2.command() == httpmessage::Mess::Command::Mess_Command_PAUSE);
    ASSERT_STREQ(mes2.url().c_str(), url);
}
