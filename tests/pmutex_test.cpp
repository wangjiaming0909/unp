#include "pmutex_test.h"

using namespace TEST;
#include "../server/thread/pmutex.h"
#include "../server/thread/unp_pthread.h"
#include <boost/function.hpp>
#include <iostream>

void thread_call(){
    std::cout << 123 << std::endl;
}

void PMUTEX_TEST::test_constructor(){
    // thread::pmutex mutex;
    boost::function<void()> func = &thread_call;
    thread::unp_pthread thread1(func, "thread1");
    thread1.start();
}

