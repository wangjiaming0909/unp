#ifndef _UNP_THREAD_H_
#define _UNP_THREAD_H_

#include "../util/XString.h"
#include "pmutex.h"
#include "stdmutex.h"
#include "pcondition.h"
namespace thread
{
#ifdef USING_PTHREAD
typedef pmutex          MutexType;
typedef pcondition      ConditionType;
#else
typedef stdmutex MutexType;
#endif 

class Thread
{
public:

public:
    Thread(){}

private:
    MutexType       m_mutex;
    // ConditionType   m_condition;
};

} // thread

#endif // _UNP_THREAD_H_