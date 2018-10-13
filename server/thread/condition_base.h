#ifndef _CONDITION_BASE_H_
#define _CONDITION_BASE_H_

#include "mutex_base.h"

namespace thread{

template <typename CONDITION_TYPE,
        typename THREAD_ID_TYPE,
        typename MUTEX_IMP_TYPE>
class condition_base{
public:
    typedef mutex_base<THREAD_ID_TYPE, MUTEX_IMP_TYPE> MutexType;
    condition_base(MutexType* mutex) : m_mutex(mutex){}
    virtual ~condition_base(){}
    virtual void wait() = 0;
    virtual void notify() = 0;

protected:
    MutexType*      m_mutex;
    CONDITION_TYPE*  m_condition;
};
}

#endif
