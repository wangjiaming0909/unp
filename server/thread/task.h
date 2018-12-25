#ifndef _UNP_THREAD_TASK_H_
#define _UNP_THREAD_TASK_H_
#include "server/thread/thread_pool.h"
#include "server/thread/message_queue.h"

namespace thread{

//type T is the type of data
template <typename T>
class task{
public:
    using micro_seconds = std::chrono::microseconds; 
    using mq_type = message_queue<T>;
    task(thread_pool* t_pool, mq_type* msg_q);
    thread_pool* th_pool() {return t_pool_p_; }
    message_queue* mq() { return msg_queue_p_; }

    virtual int activate();
    virtual int wait();

   virtual int routine() = 0;
   static int routine_run(task* self);
private:
    //why delete ?
//    task(const task&) = delete;
//    task& operator=(const task&) = delete;
private:
    thread_pool*                t_pool_p_;
    mq_type*                    msg_queue_p_;
};


}//namespace thread

#endif // _UNP_THREAD_TASK_H_
