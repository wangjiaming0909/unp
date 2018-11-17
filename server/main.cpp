#include <iostream>
#include <exception>
#include <boost/assert.hpp>
#include <cstdlib>
#include <fstream>
#include "config/ServerConfig.h"
#include "util/easylogging++.h"
#include "thread/thread_pool.h"
#include "Server.h"
#include "net/sock_connector.h"

int print_Argc_Argv(int argc, char** argv);
void setupLogger();
// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv){

    using namespace std;
    using namespace config;
    setupLogger();
    LOG(INFO) << "Server Started";

    net::sock_connector connector{};
    // net::inet_sock fd{net::sock_type::stream, 0};
    net::inet_sock fd2{};
    net::sock_stream new_stream{&fd2};
    net::inet_addr server_addr{80, "192.168.0.2"};
    microseconds timeout = 5s;
    int ret = connector.connect(new_stream, server_addr, &timeout, 1, 0);
    if(ret != 0) 
        LOG(ERROR) << strerror(errno);
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
    LOG(INFO) << "buffer: " << buffer;
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
    print_Argc_Argv(argc, argv);
    LOG(INFO) << "Server is going to end";
    return 0;
}

void setupLogger(){
    using namespace el;
    Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.set(Level::Global, ConfigurationType::Enabled, "true");
    defaultConf.set(Level::Global, ConfigurationType::Format, "%datetime, %thread, %level, %file, %line, %func, %msg");
    defaultConf.set(Level::Global, ConfigurationType::PerformanceTracking, "false");
    defaultConf.set(Level::Global, ConfigurationType::ToFile, "true");
    defaultConf.set(Level::Global, ConfigurationType::ToStandardOutput, "true");
    defaultConf.set(Level::Global, ConfigurationType::Filename, "./log");
    defaultConf.set(Level::Global, ConfigurationType::LogFlushThreshold, "100");
    defaultConf.set(Level::Global, ConfigurationType::MaxLogFileSize, "2097152");

    Loggers::reconfigureAllLoggers(defaultConf);
}

int print_Argc_Argv(int argc, char** argv){
    using namespace std;
    if(argc == 0){
        cout << "no parameters" << endl;
        return 0;
    } 

    cout << "argc: " << argc << endl;
    for(int i = 0; i < argc; i++){
        try{
            BOOST_ASSERT(argv + i != nullptr && "argc is matching the number of argv");
            cout << *(argv + i) << " ";
        }
        catch(std::exception e){
            std::cout << "exception catched" << std::endl; 
            std::cout << e.what() << std::endl;
            return -1;
        }
   }
   cout << endl;
   return 0;
}
