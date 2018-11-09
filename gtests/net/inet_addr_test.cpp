#include <gtest/gtest.h>
#include "../../server/net/inet_addr.h"
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstdio>
#include <arpa/inet.h>
#include <netinet/in.h>

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
    ASSERT_EQ(ret_addr.sin_port, 0);
}

TEST(test_string_to_addr, give_wrong_port){
    boost::scoped_array<char> zero_port{new char[16]};
    snprintf(zero_port.get(), 16, "%s", "127.0.0.1:99999");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(zero_port.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    ASSERT_EQ(ret, -1);
    boost::scoped_array<char> ip_str{new char[16]};
    memset(ip_str.get(), 0, 16);
    const char* ret_addr_ptr = inet_ntop(AF_INET, &(ret_addr.sin_addr), ip_str.get(), sizeof(sockaddr_in));
    ret = memcmp("127.0.0.1", ret_addr_ptr, strlen(ret_addr_ptr));
    ASSERT_EQ(ret_addr.sin_port, 0);
}

TEST(test_addr_to_string, give_INADDR_ANY_should_return_false){
    const int buffer_size = INET_ADDRSTRLEN;
    boost::scoped_array<char> buffer{new char[buffer_size]};
    memset(buffer.get(), 0, buffer_size);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(22);
    int ret = net::addr_to_string(buffer.get(), buffer_size, &addr, AF_INET);
    ASSERT_EQ(ret, 0);
    ret = strcmp("0.0.0.0:22", buffer.get());
    ASSERT_EQ(ret, 0);
}

TEST(test_addr_to_string, give_right_sockaddr_in_struct){
    const int buffer_size = INET_ADDRSTRLEN;
    boost::scoped_array<char> buffer{new char[buffer_size]};
    memset(buffer.get(), 0, buffer_size);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(22);
    int ret = net::addr_to_string(buffer.get(), buffer_size, &addr, AF_INET);
    std::cout << strerror(errno) << std::endl;
    ASSERT_EQ(ret, 0);
    ret = strcmp("127.0.0.1:22", buffer.get());
    ASSERT_EQ(ret, 0);
}

TEST(test_inet_addr_class, construct_a_inet_addr){
    boost::scoped_ptr<net::inet_addr> inaddr{new net::inet_addr(22, "127.0.0.1", AF_INET)};
    auto addr_str = inaddr->to_string();
    ASSERT_EQ(*addr_str, "127.0.0.1:22");
}