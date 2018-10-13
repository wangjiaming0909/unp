/*************************************************************************
	> File Name: pmutex.h
	> Author: 
	> Mail: 
	> Created Time: Fri 12 Oct 2018 08:27:35 PM CST
 ************************************************************************/

#ifndef _PMUTEX_H
#define _PMUTEX_H

#include <pthread.h>
#include "mutex_base.h"
#include <sys/syscall.h>
#include <unistd.h>

namespace thread{

class pmutex : mutex_base<pthread_t, pthread_mutex_t>{
public:
    pmutex(){
        pthread_mutex_init(&m_mutex_imp, NULL);
    }
    virtual void lock(){
        pthread_mutex_lock(&m_mutex_imp);
        assign_holder();
    } 
    virtual void unlock(){
        unassign_holder();
        pthread_mutex_unlock(&m_mutex_imp);
    }
    virtual pthread_mutex_t* get_imp_mutex(){
        return &m_mutex_imp;
    }
protected:
    virtual void assign_holder(){
        m_holder_thread = ::syscall(SYS_gettid);
    }
    virtual void unassign_holder(){
        m_holder_thread = 0;
    }
};
}

#endif
