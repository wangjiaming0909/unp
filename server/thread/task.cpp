#include "task.h"

using namespace thread;

template <typename T>
task<T>::task(thread_pool* t_pool, mq_type* msg_q) 
    : t_pool_p_(t_pool)
    , msg_queue_p_(msg_q){ }

//pass routine_run to the threads
template <typename T>
int task<T>::activate(int thread_count){
    for(int i = 0; i < thread_count; i++){
        t_pool_p_->add_task(std::bind(task::routine_run, this));
    }
    return thread_count;
}

template <typename T>
int task<T>::wait(){
    t_pool_p_->stop();
    return 0;
}

template <typename T>
int task<T>::routine_run(task* self){
    return self->routine();
}

template <typename T>
int task<T>::put_data(data_type*& data, const micro_seconds* timeout){
    return msg_queue_p_->enqueue_head(data, timeout);
}

template <typename T>
int task<T>::get_data(data_type*& data, const micro_seconds* timeout){
    return msg_queue_p_->dequeue_tail(data, timeout);
}