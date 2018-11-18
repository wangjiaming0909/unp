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
   server(){} 
   ~server() override{}
    void SetUp() override{
        server_addr_ = inet_addr{};
        server_addr_.set_port_number(9090);
        acceptor_ = net::sock_acceptor(server_addr_, 1);
    }
    void TearDown() override{}
    net::sock_acceptor acceptor_;
    inet_addr server_addr_;
};

TEST_F(server, as_a_server){
    inet_sock client_fd{};
    inet_addr client_addr{};
    sock_stream client_stream{&client_fd};
    std::chrono::microseconds timeout = 5s;
    int ret = acceptor_.accept(client_stream, &client_addr, 0);
    if(ret != 0) LOG(INFO) << strerror(errno);
    ASSERT_TRUE(ret == 0);
}
