#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "util/easylogging++.h"

void setupLogger();
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv){
  setupLogger();
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

void setupLogger(){
  using namespace el;
  Configurations defaultConf;
  defaultConf.setToDefault();
  defaultConf.set(Level::Global, ConfigurationType::Enabled, "true");
  //%func
  defaultConf.set(Level::Global, ConfigurationType::Format, "%datetime, %thread, %level, %file, %line, %msg");
  defaultConf.set(Level::Global, ConfigurationType::PerformanceTracking, "false");
  defaultConf.set(Level::Global, ConfigurationType::ToFile, "false");
  defaultConf.set(Level::Global, ConfigurationType::ToStandardOutput, "true");
  defaultConf.set(Level::Global, ConfigurationType::Filename, "./log");
  defaultConf.set(Level::Global, ConfigurationType::LogFlushThreshold, "100");
  defaultConf.set(Level::Global, ConfigurationType::MaxLogFileSize, "10485760");

  Loggers::reconfigureAllLoggers(defaultConf);
  Loggers::reconfigureAllLoggers(defaultConf);
  el::Loggers::addFlag(LoggingFlag::LogDetailedCrashReason);
  el::Loggers::addFlag(LoggingFlag::ColoredTerminalOutput);
}
