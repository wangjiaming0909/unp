/*************************************************************************
	> File Name: latch.h
	> Author: 
	> Mail: 
	> Created Time: Sat 13 Oct 2018 09:27:14 AM CST
 ************************************************************************/

#ifndef _LATCH_H
#define _LATCH_H
#include "mutex_base.h"
#include "condition_base.h"

namespace thread{
template <typename CONDITION_TYPE,
        typename THREAD_ID_TYPE,
        typename MUTEX_IMP_TYPE>
class latch{
public:
    typedef mutex_base<THREAD_ID_TYPE, MUTEX_IMP_TYPE> MutexType;
    typedef condition_base<CONDITION_TYPE, THREAD_ID_TYPE, MUTEX_IMP_TYPE> ConditionType;
    explicit latch(int count){
        
    }
    void wait(){

    }
    void countDown(){

    }
    void getCount() const {

    }
    
private:
    ConditionType* m_condition;
    MutexType* m_mutex;
    int m_count;
};
}
#endif

