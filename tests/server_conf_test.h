/*************************************************************************
	> File Name: server_conf_test.h
	> Author: 
	> Mail: 
	> Created Time: Wed 03 Oct 2018 08:07:22 PM CST
 ************************************************************************/

#ifndef _SERVER_CONF_TEST_H
#define _SERVER_CONF_TEST_H
#include "./tests_macros.h"
#include <typeinfo>
#include "../server/config/ServerConfig.h"

namespace TEST{
class SERVER_CONFIG_TEST{
public:
    SERVER_CONFIG_TEST(){
        m_config_ptr = new config::ServerConfig();
    }
    static void test(){
        SERVER_CONFIG_TEST test;
        CONSOLE_LOG(typeid(test).name() << "---");
        test.test_constructor();
        test.test_options();
        test.test_if_no_this_option();
    }
public:
    void test_constructor();
    void test_options();
    void test_if_no_this_option();

private:
    config::ServerConfig* m_config_ptr;
};
}
#endif
