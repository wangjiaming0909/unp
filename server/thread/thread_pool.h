#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_
#include "thread.h"
#include <boost/thread.hpp>
#include "../util/XString.h"
#include <boost/ptr_container/ptr_vector.hpp>

namespace thread {
class Thread_Pool{
public:
    Thread_Pool(const util::string& name = util::string("threadPool"));
    ~Thread_Pool();
    void setMaxQueueSize(int maxSize){m_maxQueueSize = maxSize;}
    const util::string& name() const{return m_thread_name;}
    size_t queueSize() const;
private:
    bool isFull() const;
    void runInThread();
private:
    util::string m_thread_name;
    boost::ptr_vector<boost::thread> m_threads;
    bool m_running;
    int m_maxQueueSize;
};
}

#endif
