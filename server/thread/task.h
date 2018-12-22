#ifndef _UNP_THREAD_TASK_H_
#define _UNP_THREAD_TASK_H_
#include "server/thread/thread_pool.h"
#include "server/thread/message_queue.h"

namespace thread{

class task{
public:
    task(thread_pool* t_pool, message_queue* msg_q) 
        : t_pool_p_(t_pool)
        , msg_queue_p_(msg_q){ }
private:
    thread_pool*                t_pool_p_;
    message_queue*              msg_queue_p_;
};


}//namespace thread

#endif // _UNP_THREAD_TASK_H_