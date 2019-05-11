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
#include "server/reactor/read_write_handler.h"
#include "server/reactor/poll_reactor_impl.h"
#include "server/util/min_heap.h"
#include "server/reactor/tcp_server.h"
#include <exception>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <unistd.h>

#include <queue>

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP
using namespace reactor;
using namespace thread;
using namespace net;
using namespace std::chrono_literals;

int set_reactor_acceptor(const char* ipAddr, in_port_t port){
    thread_pool pool{10};
    pool.start();
    message_queue<int> mq{};
    // reactor::Reactor rt{new reactor::select_reactor_impl{}};
    reactor::Reactor rt{new reactor::poll_reactor_impl{}};
    inet_addr listen_addr{port, ipAddr};
    reactor_acceptor acceptor{rt, pool, listen_addr};

    int ret = 0;
    for(;;)
    {
        ret = rt.handle_events();
        if(ret != 0)
        {
            LOG(ERROR) << "handle_events error...";
            break;
        }
    }
	return 0;
}

int set_reactor_acceptor_without_pool(const char* ipAddr, in_port_t port)
{
    reactor::Reactor rt{new reactor::poll_reactor_impl{}};
    inet_addr listen_addr{port, ipAddr};
    reactor::acceptor* accptor = new acceptor{rt, listen_addr};

    int ret = 0;
    for (;;)
    {
        ret = rt.handle_events();
        if(ret != 0)
        {
            LOG(ERROR) << "handle_events error...";
            break;
        }
    }
    while(accptor->destroy_acceptor() != 0)
    {

    }
	return 0;
}

int set_reactor_acceptor_using_epoll(const char* ipAddr, in_port_t port){
    thread_pool pool{10};
    // pool.start();
    message_queue<int> mq{};
    // reactor::Reactor rt{new reactor::select_reactor_impl{}};
    reactor::Reactor rt{new reactor::epoll_reactor_impl{}};
    inet_addr listen_addr{port, ipAddr};
    reactor_acceptor acceptor{rt, pool, listen_addr};

    int ret = 0;
    for(;;)
    {
        ret = rt.handle_events();
        if(ret != 0)
        {
            LOG(ERROR) << "handle_events error...";
            break;
        }
    }
	return 0;
}

int set_reactor_connector(const char* ipAddr, in_port_t port){
    thread_pool pool{10};
    pool.start();
    message_queue<int> mq{};
    // reactor::Reactor rt{new reactor::select_reactor_impl{}};
    reactor::Reactor rt{new reactor::poll_reactor_impl{}};

    // //acceptor
    // inet_addr listen_addr{9000, "192.168.0.112"};
    // reactor_acceptor acceptor{rt, pool, listen_addr};

    //connector
    // inet_addr remote_addr{9000, "192.168.0.112"};
    inet_addr remote_addr{port, ipAddr};
    // reactor_connector<int, ReadHandler<int>> connector{rt, pool, mq};
    reactor_connector<int, read_write_handler<int>> connector{rt, pool, mq};
    
    int ret = connector.connect(remote_addr, 5s);
    if(ret != 0) return ret;

    
    std::chrono::microseconds timeout = 5s;
    while(true){
        ret = rt.handle_events(&timeout);
        if(ret < 0) return -1;
    }
}

int multi_connector(const char* ipAddr, in_port_t port, int sockets_count, int interval/*seconds*/){
	(void)sockets_count;
	(void)interval;
	
    thread_pool pool{10};
    pool.start();
    message_queue<int> mq{};
    // reactor::Reactor rt{new reactor::select_reactor_impl{}};
    reactor::Reactor rt{new reactor::poll_reactor_impl{}};

    //connector
    // inet_addr remote_addr{9000, "192.168.0.112"};
    inet_addr remote_addr{port, ipAddr};
    std::vector<reactor_connector<int, read_write_handler<int>>*> connectors{};

        //::sleep(interval);
        auto* connector = new reactor_connector<int, read_write_handler<int>>{rt, pool, mq};
        connectors.push_back(connector);
        connector->connect(remote_addr, 5s);
    
    std::chrono::microseconds timeout = 5s;
    while(true){
        rt.handle_events(&timeout);
    }
    
}

int main(int argc, char** argv){

    using namespace std;
    server_scoped_helper s_h{argc, argv};


    inet_addr local_addr{9090, "192.168.0.112"};
    reactor::tcp_server server{local_addr};
    server.set_thread_num(8);
    server.open();
    // set_reactor_acceptor_without_pool("192.168.0.112", 9090);


    /*
    if(argc == 4){
        const char* ipAddr = argv[2];
        int port = atoi(argv[3]);
        if(strcmp(argv[1], "-listen")== 0){
//             set_reactor_acceptor(ipAddr, port);
//            set_reactor_acceptor_using_epoll(ipAddr, port);
            set_reactor_acceptor_without_pool(ipAddr, port);
        }
        if(strcmp(argv[1], "-connect")== 0){
            set_reactor_connector(ipAddr, port);
            // multi_connector(ipAddr, port, 4, 1);
        }
    }else{
        LOG(ERROR) << "args error.....";
    }
    */

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
