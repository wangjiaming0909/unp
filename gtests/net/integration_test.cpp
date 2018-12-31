#include <gtest/gtest.h>
#include "../../server/net/sock_connector.h"
#include "../../server/net/sock_acceptor.h"

using namespace net;
class client : public testing::Test{
protected:
    client(){}
    ~client() override{}
    void SetUp() override{

    }
    void TearDown() override{}
};

class server : public testing::Test{
protected:
   server() : server_addr_(), acceptor_(server_addr_){} 
   ~server() override{}
    void SetUp() override{
        server_addr_ = inet_addr{};
        server_addr_.set_port_number(9090);
        acceptor_ = net::sock_acceptor(server_addr_, 1);
    }
    void TearDown() override{}
    inet_addr server_addr_;
    net::sock_acceptor acceptor_;
};

TEST_F(server, as_a_server){
    inet_addr client_addr{};
    sock_stream client_stream{};
    std::chrono::microseconds timeout = 5s;
    int ret = acceptor_.accept(client_stream, &client_addr, &timeout);
    // char buffer[100];
    // while(true){
    //     sleep(1);
    //     memset(buffer, 0, 100);
    //     int len = client_stream.read(buffer, 100);
    //     client_stream.send(buffer, len, 0);
    // }
    // if(ret != 0) LOG(INFO) << strerror(errno);
    ASSERT_TRUE(ret != 0);
}
