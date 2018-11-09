#include <gtest/gtest.h>
#include "../../server/net/inet_addr.h"
#include <boost/scoped_array.hpp>
#include <cstdio>
#include <arpa/inet.h>

TEST(test_string_to_addr, give_the_right_ip_and_port){
    boost::scoped_array<char> ip_and_port{new char[32]};
    snprintf(ip_and_port.get(), 32, "%s", "127.0.0.1:22");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(ip_and_port.get(), &ret_addr, AF_INET);
    if(ret != 0) GTEST_FAIL();
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    ASSERT_EQ(ret_addr.sin_port, htons(22));
    boost::scoped_array<char> ip_str{new char[16]};
    memset(ip_str.get(), 0, 16);
    const char* ret_ptr = inet_ntop(AF_INET, &(ret_addr.sin_addr), ip_str.get(), sizeof(sockaddr_in));
    if(ret_ptr == nullptr) FAIL();
    ret = memcmp("127.0.0.1", ret_ptr, strlen(ret_ptr));
    if(ret != 0) FAIL();
}

TEST(test_string_to_addr, give_wrong_ip_or_port){
    boost::scoped_array<char> wrong_ip_and_port{new char[16]};
    snprintf(wrong_ip_and_port.get(), 16, "%s", "127.2.2.1234:22");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(wrong_ip_and_port.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    ASSERT_EQ(ret_addr.sin_port, htons(22));
    //assert that addr is not right
}
