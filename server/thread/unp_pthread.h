/*************************************************************************
	> File Name: unp_pthread.h
	> Author: 
	> Mail: 
	> Created Time: Sat 13 Oct 2018 11:47:15 AM CST
 ************************************************************************/

#ifndef _UNP_PTHREAD_H
#define _UNP_PTHREAD_H

#include "thread_base.h"
#include "pmutex.h"
#include "pcondition.h"

void* thread_cb(void*){

}


namespace thread{
class unp_pthread : Thread<pcondition, pthread_t, pthread_mutex_t>{
public:
    typedef Thread<pcondition, pthread_t, pthread_mutex_t> BaseType;
    unp_pthread(const ThreadFunc& func, util::string thread_name) : BaseType(func, thread_name){}
    ~unp_pthread(){}
    virtual void start(){
        m_started = true;
        if(pthread_create(&m_thread_id, nullptr, &thread_cb, nullptr)){
            m_started = false;
        }else{
            m_latch.wait();
        }
    }
    virtual int join(){
        m_joined = true;
        return pthread_join(m_thread_id, nullptr);
    }
private:
};

}

#endif
