#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../server/util/easylogging++.h"

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
    defaultConf.set(Level::Global, ConfigurationType::ToFile, "true");
    defaultConf.set(Level::Global, ConfigurationType::ToStandardOutput, "true");
    defaultConf.set(Level::Global, ConfigurationType::Filename, "./log");
    defaultConf.set(Level::Global, ConfigurationType::LogFlushThreshold, "100");
    defaultConf.set(Level::Global, ConfigurationType::MaxLogFileSize, "2 * 1024 * 1024");

    Loggers::reconfigureAllLoggers(defaultConf);
}
