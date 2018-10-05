#ifndef SERVER_TEST_H
#define SERVER_TEST_H

#include "tests_macros.h"
#include "../server/Server.h"
namespace TEST{

class SERVER_TEST
{
public:
    SERVER_TEST();
    ~SERVER_TEST();
    static void test(){
        SERVER_TEST test;
        CONSOLE_LOG(typeid(test).name() << "---");
        test.if_non_valid_config_file_passed_in();
        test.should_initialized_well();
        test.should_be_started();
        test.should_be_stoped();
    }
public:
    void if_non_valid_config_file_passed_in();
    void should_initialized_well();
    void should_be_started();
    void should_be_stoped();
private:
    server::Server* m_server_ptr = nullptr;
};
}
#endif // SERVER_TEST_H
