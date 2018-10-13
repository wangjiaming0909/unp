#ifndef _UNP_THREAD_BASE_H_
#define _UNP_THREAD_BASE_H_

#include "../util/XString.h"
#include "pmutex.h"
#include "stdmutex.h"
#include "pcondition.h"
#include "latch.h"
#include <boost/function.hpp>
namespace thread
{
//#ifdef USING_PTHREAD
//typedef pmutex          MutexType;
//typedef pcondition      ConditionType;
//#else
//typedef stdmutex MutexType;
//#endif 

template <typename CONDITION_TYPE,
        typename THREAD_ID_TYPE,
        typename MUTEX_IMP_TYPE>
class Thread
{
public:
    typedef latch<CONDITION_TYPE, THREAD_ID_TYPE, MUTEX_IMP_TYPE>   LatchType;
    typedef boost::function<void* (void*)>                                ThreadFunc;
    
public:
    Thread(const ThreadFunc& func, const util::string& thread_name) 
        : m_started(false),
        m_joined(false),
        m_thread_id(0),
        m_thread_name(thread_name),
        m_func(func),
        m_latch(1)
    {}
    virtual ~Thread(){}
    virtual void start() = 0;
    virtual int join() = 0;
    bool started()const{return m_started;}
    const util::string thread_name() const {return m_thread_name;}

protected:
    bool                m_started;
    bool                m_joined;
    THREAD_ID_TYPE      m_thread_id;
    util::string        m_thread_name;
    ThreadFunc&         m_func;
    LatchType           m_latch;
};

} // thread

#endif // _UNP_THREAD_BASE_H_
