/*************************************************************************
	> File Name: server_conf_test.cpp
	> Author: 
	> Mail: 
	> Created Time: Wed 03 Oct 2018 08:10:22 PM CST
 ************************************************************************/
#include "server_conf_test.h"

TEST::SERVER_CONFIG_TEST::SERVER_CONFIG_TEST()
{
    m_config_ptr = new config::ServerConfig();
}

void TEST::SERVER_CONFIG_TEST::test_constructor(){
//    config::ServerConfig config;
    assert(m_config_ptr->getReadConfigFileStatus() == true);
//    util::string port = "port";
//    assert(config[port] == "\"9999\"");
    ASSERT_OK;
}

void TEST::SERVER_CONFIG_TEST::test_options()
{
//    config::ServerConfig config;
	ASSERT_OK;
}
