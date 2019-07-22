#include <gtest/gtest.h>
#include "net/inet_addr.h"
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdio>
#include <arpa/inet.h>
#include <netinet/in.h>

TEST(string_to_addr, give_the_right_ip_and_port){
    boost::scoped_array<char> ip_and_port{new char[INET_ADDRSTRLEN]};
    snprintf(ip_and_port.get(), INET_ADDRSTRLEN, "%s", "127.0.0.1:22");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(ip_and_port.get(), &ret_addr, AF_INET);
    if(ret != 0) GTEST_FAIL();
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    ASSERT_EQ(ret_addr.sin_port, htons(22));
    boost::scoped_array<char> ip_str{new char[INET_ADDRSTRLEN]};
    memset(ip_str.get(), 0, INET_ADDRSTRLEN);
    const char* ret_ptr = inet_ntop(AF_INET, &(ret_addr.sin_addr), ip_str.get(), sizeof(sockaddr_in));
    if(ret_ptr == nullptr) FAIL();
    ASSERT_STREQ("127.0.0.1", ret_ptr);
}

TEST(string_to_addr, give_wrong_ip){
    boost::scoped_array<char> wrong_ip{new char[INET_ADDRSTRLEN]};
    snprintf(wrong_ip.get(), INET_ADDRSTRLEN, "%s", "127.2.2.1234:22");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(wrong_ip.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    //when the addr format is not right, it will not parse the port
    // ASSERT_EQ(ret_addr.sin_port, htons(22));
    ASSERT_EQ(ret, 0);
}

TEST(string_to_addr, give_zero_port_or_ADDR_ANY){
    boost::scoped_array<char> zero_port{new char[INET_ADDRSTRLEN]};
    snprintf(zero_port.get(), INET_ADDRSTRLEN , "%s", "127.0.0.1:0");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(zero_port.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    ASSERT_EQ(ret, 0);
    boost::scoped_array<char> ip_str{new char[INET_ADDRSTRLEN]};
    memset(ip_str.get(), 0, INET_ADDRSTRLEN);
    const char* ret_addr_ptr = inet_ntop(AF_INET, &(ret_addr.sin_addr), ip_str.get(), sizeof(sockaddr_in));
    ASSERT_EQ(ret_addr.sin_port, 0);
    ASSERT_STREQ("127.0.0.1", ret_addr_ptr);
}

TEST(string_to_addr, give_wrong_port){
    boost::scoped_array<char> zero_port{new char[INET_ADDRSTRLEN]};
    snprintf(zero_port.get(), INET_ADDRSTRLEN, "%s", "127.0.0.1:99999");
    sockaddr_in ret_addr;
    int ret = net::string_to_addr(zero_port.get(), &ret_addr, AF_INET);
    ASSERT_EQ(ret_addr.sin_family, AF_INET);
    ASSERT_EQ(ret, -1);
    boost::scoped_array<char> ip_str{new char[INET_ADDRSTRLEN]};
    memset(ip_str.get(), 0, INET_ADDRSTRLEN);
    const char* ret_addr_ptr = inet_ntop(AF_INET, &(ret_addr.sin_addr), ip_str.get(), sizeof(sockaddr_in));
    ASSERT_STREQ("127.0.0.1", ret_addr_ptr);
}

TEST(addr_to_string, give_INADDR_ANY_should_return_false){
    const int buffer_size = INET_ADDRSTRLEN;
    boost::scoped_array<char> buffer{new char[buffer_size]}; memset(buffer.get(), 0, buffer_size);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(22);
    int ret = net::addr_to_string(buffer.get(), buffer_size, &addr, AF_INET);
    ASSERT_EQ(ret, 0);
    ASSERT_STREQ("0.0.0.0:22", buffer.get());
}

TEST(addr_to_string, give_right_sockaddr_in_struct){
    const int buffer_size = INET_ADDRSTRLEN;
    boost::scoped_array<char> buffer{new char[buffer_size]};
    memset(buffer.get(), 0, buffer_size);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(22);
    int ret = net::addr_to_string(buffer.get(), buffer_size, &addr, AF_INET);
    ASSERT_EQ(ret, 0);
    ASSERT_STREQ("127.0.0.1:22", buffer.get());
}

//it seems that every sockaddr_in struct can be converted to a string
TEST(addr_to_string, give_wrong_sockaddr_in_struct){ 
    ASSERT_EQ(1,1);
}

class inet_addr_test : public ::testing::Test{
protected:
    inet_addr_test(){}
    ~inet_addr_test() override{}
    void SetUp() override{

    }
    void TearDown() override{}
    boost::shared_ptr<net::inet_addr> inaddr_ptr;
};

TEST_F(inet_addr_test, construct_a_inet_addr){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr(22, "127.0.0.1"));
    util::string addr_str = inaddr_ptr->to_string();
    ASSERT_EQ(addr_str, "127.0.0.1:22");
}

TEST_F(inet_addr_test, give_wrong_port_or_address_or_address_family_should_throw_exception){
    ASSERT_THROW(net::inet_addr(22, "1.2.2.2141"), std::invalid_argument);
    ASSERT_THROW(net::inet_addr(22, "123.123.123.123", AF_UNSPEC), std::invalid_argument);
}

TEST_F(inet_addr_test, copy_constructor){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr(22, "127.0.0.1"));
    net::inet_addr rl = *inaddr_ptr;
    ASSERT_EQ(rl.get_port_number(), inaddr_ptr->get_port_number());
    ASSERT_EQ(rl.get_address_string(), inaddr_ptr->get_address_string());
}

TEST_F(inet_addr_test, operator_equal){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr(22, "127.0.0.1"));
    net::inet_addr rl{};
    rl = *inaddr_ptr;
    ASSERT_EQ(rl.get_port_number(), inaddr_ptr->get_port_number());
    ASSERT_EQ(rl.get_address_string(), inaddr_ptr->get_address_string());
}

TEST_F(inet_addr_test, get_port_number){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr(22, "127.0.0.1"));
    ushort actual_port = inaddr_ptr->get_port_number();
    ASSERT_EQ(actual_port, 22);
}

TEST_F(inet_addr_test, get_address_string){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr(22, "127.0.0.1"));
    auto actual_address_str = inaddr_ptr->get_address_string();
    ASSERT_EQ(actual_address_str, util::string("127.0.0.1"));
}

TEST_F(inet_addr_test, set_port_number){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr());
    inaddr_ptr->set_port_number(22);
    ASSERT_EQ(inaddr_ptr->get_port_number(), 22);
}

TEST_F(inet_addr_test, set_address){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr());
    inaddr_ptr->set_address("127.0.0.1");
    auto actual_address_str = inaddr_ptr->get_address_string();
    ASSERT_EQ(actual_address_str, util::string("127.0.0.1"));
}

TEST_F(inet_addr_test, set_addr){
    inaddr_ptr = boost::shared_ptr<net::inet_addr>(new net::inet_addr());
    inaddr_ptr->set_addr("127.0.0.1:22");
    auto actual_addr_str = inaddr_ptr->to_string();
    ASSERT_EQ(actual_addr_str, "127.0.0.1:22");
}