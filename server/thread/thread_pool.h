#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_
#include "thread_base.h"
namespace thread {
#ifdef USING_PTHREAD
typedef pthread_t THREAD_ID_TYPE;
typedef pthread_mutex_t MUTEX_IMP_TYPE;
typedef pthread_cond_t  CONDITION_IMP_TYPE;
#endif
class Thread_Pool{
public:
    Thread_Pool();
private:
    Thread<CONDITION_IMP_TYPE, THREAD_ID_TYPE, MUTEX_IMP_TYPE> * m_thread;
};
}

#endif
