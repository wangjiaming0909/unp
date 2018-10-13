#ifndef _PCONDITION_H_
#define _PCONDITION_H_

#include "condition_base.h"
#include <pthread.h>

namespace thread{

class pcondition
 : condition_base<pthread_cond_t,
                  pthread_t,
                  pthread_mutex_t>{
public:
    typedef condition_base<pthread_cond_t, pthread_t, pthread_mutex_t> BaseType;
    typedef mutex_base<pthread_t, pthread_mutex_t> MutexType;
    explicit pcondition(MutexType* mutex) : BaseType(mutex){
        pthread_cond_init(&m_condition, nullptr);
    }
    ~pcondition(){
        pthread_cond_destroy(&m_condition);
    }
    virtual void wait(){
        pthread_cond_wait(&m_condition, m_mutex->get_imp_mutex());
    }
    virtual void nofity(){
        pthread_cond_signal(&m_condition);
    }
};
}

#endif
