
#include "pthread_test.h"
#include <boost/thread.hpp>
#include "util/easylogging++.h"

using namespace TEST;

void* add_call(void *p){
    if(p != nullptr){
        int* intptr = static_cast<int*>(p);
        for(int i = 0; i < 10000; i++){
            (*intptr)++;
            // LOG(INFO) << *intptr;
        }
        return static_cast<void*>(intptr);
    }
    return nullptr;
}

void* minus_call(void *ptr){
    if(ptr != nullptr){
        int* intptr = static_cast<int*>(ptr);
        for(int i = 0; i < 10000; i++){
            (*intptr)--;
            // LOG(INFO) << *intptr;
        }
        return static_cast<void*>(intptr);
    }
    return ptr;
}

void UNP_PTHREAD_TEST::test_new_thread(){
    int a = 0;
    boost::thread thread1{add_call, &a};
    boost::thread thread2{minus_call, &a};
    thread1.join();
    thread2.join();
    assert(a == 0);
    ASSERT_OK;
}