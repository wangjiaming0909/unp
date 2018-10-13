
#include "pthread_test.h"

using namespace TEST;

void* thread_call(void *p){
    using namespace std;
    if(p != nullptr){
        int* intptr = static_cast<int*>(p);
        (*intptr)++;
        std::cout << *intptr << std::endl;
        return static_cast<void*>(intptr);
    }
    return nullptr;
}

void UNP_PTHREAD_TEST::test_new_thread(){
    using namespace thread;
    unp_pthread thread1 = unp_pthread(thread_call, "thread1");
    thread1.start();
    int ret = thread1.join();
    assert(ret == 0);
    assert(thread1.thread_name == "thread1");
    ASSERT_OK;
}