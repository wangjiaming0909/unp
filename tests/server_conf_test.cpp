/*************************************************************************
	> File Name: server_conf_test.cpp
	> Author: 
	> Mail: 
	> Created Time: Wed 03 Oct 2018 08:10:22 PM CST
 ************************************************************************/

#include "server_conf_test.h"
#include "../server/config/ServerConfig.h"

void TEST::SERVER_CONFIG_TEST::test_constructor(){
    config::ServerConfig config;
    assert(config.getReadConfigFileStatus() == true);
    ASSERT_OK; 
}
