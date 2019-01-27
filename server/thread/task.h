#ifndef _UNP_THREAD_TASK_H_
#define _UNP_THREAD_TASK_H_
#include "server/thread/thread_pool.h"
#include "server/thread/message_queue.h"
#include "server/reactor/event_handler.h"
#include <boost/shared_ptr.hpp>

using namespace std::chrono_literals;
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
    virtual int put_data_and_activate(data_type data, const micro_seconds& timeout = 0ms);

    void cancel();

    int put_data(data_type data, const micro_seconds& timeout = 0ms);
    data_type get_data(const micro_seconds& timeout = 0ms);
    int get_data(data_type* data, const micro_seconds& timeout = 0ms);

   //在routine中，需要自己从message_queue中拿出data，然后自己做处理
   //routine 会被 routine_run 自动调用
    virtual int routine() = 0;
   //decide which event you want to register
    virtual int open() = 0;
    //for closing the handler
    virtual int close();
    int routine_run();

// public:
//     Event_Type get_event_type() const {return current_event_;}
//     void set_event_type(Event_Type event) {current_event_ = event; }
private:
    //why delete ?
//    task(const task&) = delete;
//    task& operator=(const task&) = delete;
protected:
    thread_pool*                t_pool_p_;
    mq_type*                    msg_queue_p_;
    Event_Type                  current_event_;
};

template <typename T>
task<T>::task(reactor::Reactor& react, thread_pool& t_pool, mq_type& msg_q) 
    : event_handler(react)
    , t_pool_p_(&t_pool)
    , msg_queue_p_(&msg_q)
    , current_event_(0) { }

//pass routine_run to the threads
template <typename T>
int task<T>::activate(int thread_count){
    for(int i = 0; i < thread_count; i++){
        t_pool_p_->add_task(std::bind(&task::routine_run, this));
    }
    return thread_count;
}

template <typename T>
int task<T>::put_data_and_activate(data_type data, const micro_seconds& timeout){
    int ret = put_data(data, timeout);
    if(ret != 0) return -1;
    return activate(1);
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
int task<T>::routine_run(){
    int ret = this->routine();
    if(ret != 0) {
        if(this->close() != 0) return -1;
    }
    return ret;
}

template <typename T>
int task<T>::close(){
    int handle = this->get_handle();
    if(handle == INVALID_HANDLE) return -1;
    this->msg_queue_p_->close();
    this->reactor_->unregister_handler(this->get_handle(), this, current_event_);
    return this->handle_close(0); // 0没有意义
}

template <typename T>
int task<T>::put_data(data_type data, const micro_seconds& timeout){
    return msg_queue_p_->enqueue_head(data, timeout);
}

template <typename T>
int task<T>::get_data(data_type* data, const micro_seconds& timeout){
    return msg_queue_p_->dequeue_tail(data, timeout);
}

template <typename T>
typename task<T>::data_type task<T>::get_data(const micro_seconds& timeout){
    data_type ret{};
    int res = msg_queue_p_->dequeue_tail(&ret, timeout);
    if(res != 0) LOG(INFO) << "dequeue error";
    return ret;
}

}//namespace thread

#endif // _UNP_THREAD_TASK_H_
