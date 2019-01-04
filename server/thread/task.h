#ifndef _UNP_THREAD_TASK_H_
#define _UNP_THREAD_TASK_H_
#include "server/thread/thread_pool.h"
#include "server/thread/message_queue.h"
#include "server/reactor/event_handler.h"
#include <boost/shared_ptr.hpp>

namespace thread{

//type T is the type of data
template <typename T>
class task : public reactor::event_handler{
public:
    using micro_seconds = std::chrono::microseconds; 
    using mq_type = message_queue<T>;
    using data_type = data_block<T>;
    task(reactor::Reactor& react, thread_pool& t_pool, mq_type& msg_q);
    thread_pool* th_pool() {return t_pool_p_; }
    message_queue<T>* mq() { return msg_queue_p_; }

    virtual int activate(int thread_count);
    virtual int wait(const micro_seconds* timeout = 0);

    void cancel();

    int put_data(data_type* data, const micro_seconds& timeout = micro_seconds{0});
    boost::shared_ptr<data_type> get_data(const micro_seconds& timeout = micro_seconds{0});
    int get_data(data_type* data, const micro_seconds& timeout = micro_seconds{0});

   //在routine中，需要自己从message_queue中拿出data，然后自己做处理
   //routine 会被 routine_run 自动调用
   virtual int routine() = 0;
   //decide which event you want to register
    virtual void open() = 0;
   static int routine_run(task* self);
private:
    //why delete ?
//    task(const task&) = delete;
//    task& operator=(const task&) = delete;
protected:
    thread_pool*                t_pool_p_;
    mq_type*                    msg_queue_p_;
};

template <typename T>
task<T>::task(reactor::Reactor& react, thread_pool& t_pool, mq_type& msg_q) 
    : event_handler(react)
    , t_pool_p_(&t_pool)
    , msg_queue_p_(&msg_q){ }

//pass routine_run to the threads
template <typename T>
int task<T>::activate(int thread_count){
    for(int i = 0; i < thread_count; i++){
        LOG(INFO) << "adding one task... " << i+1 << " of " << thread_count;
        LOG(INFO) << t_pool_p_;
        t_pool_p_->add_task(std::bind(&task::routine_run, this));
        LOG(INFO) << "added task";
    }
    return thread_count;
}

template <typename T>
int task<T>::wait(const micro_seconds* timeout){
    t_pool_p_->wait(timeout);
    return 0;
}

template <typename T>
void task<T>::cancel() {
    if(!t_pool_p_->cancel()){
        LOG(WARNING) << "cancel warning" << strerror(errno);
    }
}

template <typename T>
int task<T>::routine_run(task* self){
    return self->routine();
}

template <typename T>
int task<T>::put_data(data_type* data, const micro_seconds& timeout){
    return msg_queue_p_->enqueue_head(data, timeout);
}

template <typename T>
int task<T>::get_data(data_type* data, const micro_seconds& timeout){
    return msg_queue_p_->dequeue_tail(data, timeout);
}

template <typename T>
auto task<T>::get_data(const micro_seconds& timeout) -> boost::shared_ptr<data_type>{
    boost::shared_ptr<data_type> ret_ptr{new data_type{}};
    int ret = msg_queue_p_->dequeue_tail(ret_ptr.get(), timeout);
    if(ret != 0) return boost::shared_ptr<data_type>{nullptr};
    return ret_ptr;
}

}//namespace thread

#endif // _UNP_THREAD_TASK_H_
