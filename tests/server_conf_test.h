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

namespace TEST{
class SERVER_CONFIG_TEST{
public:
    static void test(){
        CONSOLE_LOG(typeid(test).name() << "---");
        SERVER_CONFIG_TEST test;
        test.test_constructor();
        
    }
public:
    void test_constructor();
};
}
#endif
