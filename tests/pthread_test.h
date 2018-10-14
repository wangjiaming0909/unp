#ifndef _UNP_PTHREAD_TEST_H_
#define _UNP_PTHREAD_TEST_H_

#include "tests_macros.h"
#include <typeinfo>

namespace TEST{
class UNP_PTHREAD_TEST{
public:
    UNP_PTHREAD_TEST(){}
    static void test(){
        UNP_PTHREAD_TEST test;
        CONSOLE_LOG(typeid(test).name() << "---");
        test.test_new_thread();
    }
private:
    void test_new_thread();
};
}
#endif