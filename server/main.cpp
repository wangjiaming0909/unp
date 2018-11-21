#include <exception>
#include <cstdlib>
#include "config/ServerConfig.h"
#include "util/easylogging++.h"
#include "thread/thread_pool.h"
#include "Server.h"
#include "net/sock_connector.h"
#include "main_helper.h"
#include "reactor/reactor.h"


// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv){

    using namespace std;
    using namespace config;
    server_scoped_helper s_h{argc, argv};

    net::sock_connector connector{};
    // net::inet_sock fd{net::sock_type::stream, 0};
    net::inet_sock fd2{};
    net::sock_stream new_stream{&fd2};
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
    "GET / HTTP/1.1\nConnection: keep-alive\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\n\n";
    ret = new_stream.send(send_buffer, strlen(send_buffer), 0, &timeout);
    LOG(INFO) << "send_buffer: " << send_buffer;
    if(ret <= 0)
        LOG(ERROR) << strerror(errno);
    ret = new_stream.read(buffer, 1024, &timeout);
    if(ret <= 0) 
        LOG(ERROR) << strerror(errno);
    LOG(INFO) << "recved_buffer: " << buffer;
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
