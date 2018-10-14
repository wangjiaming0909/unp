#ifndef _UNP_THREAD_BASE_DATA_H_
#define _UNP_THREAD_BASE_DATA_H_

#include <boost/enable_shared_from_this.hpp>
#include <pthread.h>
#include <boost/shared_ptr.hpp>

namespace thread{
struct thread_data_base;
typedef boost::shared_ptr<thread_data_base> thread_data_sptr;

struct thread_data_base : boost::enable_shared_from_this<thread_data_base>{
    pthread_t thread_id;
    thread_data_sptr self;
    bool done;
    bool join_started;
    bool joined;

    thread_data_base() : 
        thread_id(0),
        done(false),
        join_started(false),
        joined(false)
    {}
    virtual ~thread_data_base();
    virtual void run() = 0;
};
}

#endif // _UNP_THREAD_BASE_DATA_H_