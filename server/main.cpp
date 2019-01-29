#include "server/config/ServerConfig.h"
#include "server/util/easylogging++.h"
#include "server/thread/thread_pool.h"
#include "server/main_helper.h"
#include "server/reactor/reactor.h"
#include "server/reactor/read_handler.h"
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/acceptor.h"
#include "server/thread/thread_pool.h"
#include "server/reactor/connector.h"
#include <exception>
#include <cstdlib>
#include <chrono>


// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP
using namespace reactor;
using namespace thread;
using namespace net;
using namespace std::chrono_literals;

int main(int argc, char** argv){

    using namespace std;
    server_scoped_helper s_h{argc, argv};

    thread_pool pool{10};
    pool.start();
    message_queue<int> mq{};
    reactor::Reactor rt{new reactor::select_reactor_impl{}};

    // //acceptor
    // inet_addr listen_addr{9000, "192.168.0.112"};
    // reactor_acceptor acceptor{rt, pool, listen_addr};

    //connector
    inet_addr remote_addr{80, "192.168.0.2"};
    reactor_connector<int, ReadHandler<int>> connector{rt, pool, mq};
    
    connector.connect(remote_addr, 5s);
    
    std::chrono::microseconds timeout = 5s;
    rt.handle_events(&timeout);

    /*
    net::sock_connector connector{};
    // net::inet_sock fd{net::sock_type::stream, 0};
    net::sock_stream new_stream{};
    net::inet_addr server_addr{9090, "127.0.0.1"};
    microseconds timeout = 4s;
    int ret = connector.connect(new_stream, server_addr, &timeout, 1, 0);
    if(ret != 0) {
        LOG(ERROR) << strerror(errno);
        return -1;
    }
    char buffer[1024];
    memset(buffer, 0, 1024);
    char send_buffer[1024] = 
    ret = new_stream.send(send_buffer, strlen(send_buffer), 0, &timeout);
    LOG(INFO) << "send_buffer: " << send_buffer;
    if(ret <= 0)
        LOG(ERROR) << strerror(errno);
    ret = new_stream.read(buffer, 1024, &timeout);
    if(ret <= 0) 
        LOG(ERROR) << strerror(errno);
    LOG(INFO) << "recved_buffer: " << buffer;
    */
    // ServerConfig cfg;
    // server::Server server{&cfg};
    // thread::thread_pool pool{5};   
    // pool.add_task(bind(&server::Server::start, &server));
    // pool.start();
    // std::string s;
    // while(1){
    //     cin >> s;
    //     if(s == "q")
    //         exit(0);
    //     else if(s == "a"){

    //     }else
    //         cout << s << endl;
    // }
}
