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

TEST(test_string_to_addr, give_wrong_ip){
    boost::scoped_array<char> wrong_ip{new char[16]};
    snprintf(wrong_ip.get(), 16, "%s", "127.2.2.1234:22");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(wrong_ip.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    //when the addr format is not right, it will not parse the port
    // ASSERT_EQ(ret_addr.sin_port, htons(22));
    ASSERT_EQ(ret, 0);
}

TEST(test_string_to_addr, give_zero_port_or_ADDR_ANY){
    boost::scoped_array<char> zero_port{new char[16]};
    snprintf(zero_port.get(), 16, "%s", "127.0.0.1:0");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(zero_port.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    // ASSERT_EQ(ret, -1);
    boost::scoped_array<char> ip_str{new char[16]};
    memset(ip_str.get(), 0, 16);
    const char* ret_addr_ptr = inet_ntop(AF_INET, &(ret_addr.sin_addr), ip_str.get(), sizeof(sockaddr_in));
    ret = memcmp("127.0.0.1", ret_addr_ptr, strlen(ret_addr_ptr));
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(ret_addr.sin_port, INADDR_ANY);
}