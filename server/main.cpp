#include <exception>
#include <cstdlib>
#include "server/config/ServerConfig.h"
#include "server/util/easylogging++.h"
#include "server/thread/thread_pool.h"
//#include "server/Server.h"
//#include "server/net/sock_connector.h"
#include "server/main_helper.h"
#include "server/reactor/reactor.h"
#include "server/reactor/read_handler.h"
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/acceptor.h"


// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP
using namespace reactor;
using namespace thread;
using namespace net;

int main(int argc, char** argv){

    using namespace std;
    server_scoped_helper s_h{argc, argv};

    reactor::Reactor rt{new reactor::select_reactor_impl{}};
    inet_addr listen_addr{9000, "127.0.0.1"};
    reactor_acceptor acceptor{rt, listen_addr};
    
    rt.handle_events();

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
    std::string s;
    while(1){
        cin >> s;
        if(s == "q")
            exit(0);
        else if(s == "a"){

        }else
            cout << s << endl;
    }
}
