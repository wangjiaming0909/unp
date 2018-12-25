#include "task.h"

using namespace thread;

template <typename T>
task<T>::task(thread_pool* t_pool, mq_type* msg_q) 
    : t_pool_p_(t_pool)
    , msg_queue_p_(msg_q){ }

//pass routine_run to the threads
template <typename T>
int task<T>::activate(){

}

template <typename T>
int task<T>::wait(){
    // t_pool_p_->
}

template <typename T>
int task<T>::routine_run(task* self){
    return self->routine();
}