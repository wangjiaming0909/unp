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
        inet_addr server_addr;
        server_addr.set_port_number(9090);
        acceptor = net::sock_acceptor(server_addr, 1);
    }
    void TearDown() override{}
    net::sock_acceptor acceptor;
};

TEST_F(server, as_a_server){
    inet_sock client_fd{};
    inet_addr client_addr{};
    sock_stream client_stream{&client_fd};
    std::chrono::microseconds timeout = 5s;
    int ret = acceptor.accept(client_stream, &client_addr, 0);
    if(ret != 0) LOG(INFO) << strerror(errno);
    ASSERT_TRUE(ret == 0);
}
