#include "pmutex_test.h"

using namespace TEST;
#include "../server/thread/pmutex.h"
#include "../server/thread/unp_pthread.h"
#include <boost/function.hpp>
#include <iostream>
#include <unistd.h>

void* thread_call(void *p){
    int* intptr = static_cast<int*>(p);
    *intptr++;
    std::cout << *intptr << std::endl;
    return static_cast<void*>(intptr);
}

void PMUTEX_TEST::test_constructor(){
    // thread::pmutex mutex;
    //boost::function<void()> func = &thread_call;
    using namespace std;
    int a = 0;
    thread::unp_pthread thread1 = thread::unp_pthread(thread_call, "thread1");
    thread1.start();
    sleep(2);
    cout << "a: " << endl;
    
}

