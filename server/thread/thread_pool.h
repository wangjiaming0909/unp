#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_
#include "thread.h"
#include <boost/thread.hpp>
#include "../util/XString.h"
#include <boost/ptr_container/ptr_vector.hpp>

namespace thread {
//#ifdef USING_PTHREAD
//typedef pthread_t THREAD_ID_TYPE;
//typedef pthread_mutex_t MUTEX_IMP_TYPE;
//typedef pthread_cond_t  CONDITION_IMP_TYPE;
//#endif
class Thread_Pool{
public:
    Thread_Pool();
    ~Thread_Pool();
    
private:
    util::string m_thread_name;
    boost::ptr_vector<boost::thread> m_threads;
};
}

#endif
