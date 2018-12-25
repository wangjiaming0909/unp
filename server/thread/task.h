#ifndef _UNP_THREAD_TASK_H_
#define _UNP_THREAD_TASK_H_
#include "server/thread/thread_pool.h"
#include "server/thread/message_queue.h"
#include "server/reactor/event_handler.h"

namespace thread{

//type T is the type of data
template <typename T>
class task : public reactor::event_handler{
public:
    using micro_seconds = std::chrono::microseconds; 
    using mq_type = message_queue<T>;
    using data_type = data_block<T>;
    task(thread_pool* t_pool, mq_type* msg_q);
    thread_pool* th_pool() {return t_pool_p_; }
    message_queue<T>* mq() { return msg_queue_p_; }

    virtual int activate(int thread_count);
    virtual int wait();

    int put_data(data_type*& data, const micro_seconds* timeout = 0);
    int get_data(data_type*& data, const micro_seconds* timeout = 0);

   //在routine中，需要自己从message_queue中拿出data，然后自己做处理
   //routine 会被 routine_run 自动调用
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
