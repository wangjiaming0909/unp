#ifndef _UNP_PMUTEX_TEST_H_
#define _UNP_PMUTEX_TEST_H_

#include "./tests_macros.h"
#include <typeinfo>

namespace TEST{
class PMUTEX_TEST{
public:
    PMUTEX_TEST(){}
    static void test(){
        PMUTEX_TEST test;
        CONSOLE_LOG(typeid(test).name() << "---");
        test.test_constructor();
    }

public:
    void test_constructor();
};
}
#endif