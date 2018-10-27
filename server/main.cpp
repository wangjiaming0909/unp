#include <iostream>
#include <exception>
#include <boost/assert.hpp>
#include <cstdlib>
#include <fstream>
#include "config/ServerConfig.h"
#include "util/easylogging++.h"
#include "thread/thread_pool.h"
#include "Server.h"

int print_Argc_Argv(int argc, char** argv);
void setupLogger();
// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv){

    using namespace std;
    using namespace config;
    setupLogger();
    LOG(INFO) << "Server Started";
    ServerConfig cfg;
    server::Server server{&cfg};
    thread::thread_pool pool{5};   
    pool.add_task(bind(&server::Server::start, &server));
    pool.start();
    std::string s;
    while(1){
        cin >> s;
        if(s == "q")
            exit(0);
        else if(s == "a"){

        }else
            cout << s << endl;
    }
    print_Argc_Argv(argc, argv);
    LOG(INFO) << "Server is going to end";
    return 0;
}

void setupLogger(){
    using namespace el;
    Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.set(Level::Global, ConfigurationType::Enabled, "true");
    defaultConf.set(Level::Global, ConfigurationType::Format, "%datetime, %thread, %thread_name, %level, %file, %line, %func, %msg");
    defaultConf.set(Level::Global, ConfigurationType::PerformanceTracking, "false");
    defaultConf.set(Level::Global, ConfigurationType::ToStandardOutput, "true");
    defaultConf.set(Level::Global, ConfigurationType::ToFile, "true");
    defaultConf.set(Level::Global, ConfigurationType::Filename, "/tmp/unp.log");
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
