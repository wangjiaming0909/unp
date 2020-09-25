#ifndef _UNP_MAIN_HELPER_H_
#define _UNP_MAIN_HELPER_H_

#include "util/easylogging++.h"
#include "boost/assert.hpp"
#include <ostream>

void setupLogger(){
  using namespace el;
  Configurations defaultConf;
  defaultConf.setToDefault();
  defaultConf.set(Level::Global, ConfigurationType::Enabled, "true");
  defaultConf.set(Level::Global, ConfigurationType::Format, "%datetime, %thread, %file, %level, %line, %msg");
  defaultConf.set(Level::Global, ConfigurationType::PerformanceTracking, "true");
  defaultConf.set(Level::Global, ConfigurationType::ToFile, "true");
  defaultConf.set(Level::Global, ConfigurationType::ToStandardOutput, "true");
  defaultConf.set(Level::Global, ConfigurationType::Filename, "./unp.log");
  defaultConf.set(Level::Global, ConfigurationType::LogFlushThreshold, "10");
  defaultConf.set(Level::Global, ConfigurationType::MaxLogFileSize, "102400000");

  Loggers::reconfigureAllLoggers(defaultConf);
  el::Loggers::addFlag(LoggingFlag::LogDetailedCrashReason);
  el::Loggers::addFlag(LoggingFlag::ColoredTerminalOutput);
}

int print_Argc_Argv(int argc, char** argv){
  using namespace std;
  if(argc == 0){
    LOG(INFO) << "no parameters";
    return 0;
  }

  LOG(INFO) << "argc: " << argc;
  for(int i = 0; i < argc; i++){
    try{
      BOOST_ASSERT(argv + i != nullptr && "argc is matching the number of argv");
      LOG(INFO) << *(argv + i) << " ";
    }
    catch(std::exception& e){
      LOG(INFO) << "exception catched" << e.what();
      return -1;
    }
  }
  return 0;
}

class server_scoped_helper{
  public:
    server_scoped_helper(int argc, char** argv) : argc_(argc), argv_(argv){
      setupLogger();
      LOG(INFO) << "Server Started";
    }
    ~server_scoped_helper(){
      print_Argc_Argv(argc_, argv_);
      LOG(INFO) << "Server is going to end";
    }
  private:
    int argc_;
    char** argv_;
};



#endif // _UNP_MAIN_HELPER_H_
