/*************************************************************************
	> File Name: server_conf_test.cpp
	> Author:
	> Mail:
	> Created Time: Wed 03 Oct 2018 08:10:22 PM CST
 ************************************************************************/
#include "server_conf_test.h"

void TEST::SERVER_CONFIG_TEST::test_constructor(){
    assert(m_config_ptr->getReadConfigFileStatus() == true);
    ASSERT_OK;
}

void TEST::SERVER_CONFIG_TEST::test_options()
{
    util::string port = "port";
    util::string clients = "clients";
    util::string port_value = (*m_config_ptr)[port];
    util::string clients_value = (*m_config_ptr)[clients];
    assert(port_value == "\"9999\"");
    assert(clients_value == "\"1024\"");
	ASSERT_OK;
}

void TEST::SERVER_CONFIG_TEST::test_if_no_this_option(){
    util::string any_option_not_exist_in_json_file = "any_option_not_exist_in_json_file";
    util::string value = (*m_config_ptr)[any_option_not_exist_in_json_file];

    assert(value == "");
    ASSERT_OK;
}
