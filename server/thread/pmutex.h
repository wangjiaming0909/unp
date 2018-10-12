/*************************************************************************
	> File Name: pmutex.h
	> Author: 
	> Mail: 
	> Created Time: Fri 12 Oct 2018 08:27:35 PM CST
 ************************************************************************/

#ifndef _PMUTEX_H
#define _PMUTEX_H

#include <pthread.h>

namespace thread{

class pmutex{
public:
    pmutex() : m_mutex_holder(0){
        pthread_mutex_init(&m_mutex, NULL);
    }

private:
    pthread_mutex_t     m_mutex;
    pid_t               m_mutex_holder;
}   
}

#endif
