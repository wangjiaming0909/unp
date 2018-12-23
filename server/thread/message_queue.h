#ifndef _THREAD_MESSAGE_QUEUE_H_
#define _THREAD_MESSAGE_QUEUE_H_

#include <deque>
#include <mutex>
#include <atomic>
#include <memory>
#include "server/util/XString.h"
#include <chrono>
#include "server/thread/lock_guard.h"
#include <condition_variable>
#include "server/util/easylogging++.h"

namespace thread{

//used to count
class reference_count_base{
public:
    reference_count_base() : r_count_(1){}
    virtual ~reference_count_base() {}
    void release(){
        if(r_count_.fetch_sub(1) == 1) {
            dispose();
            destroy();
        }
    }
    void add_reference(){ r_count_.fetch_add(1); }
    int get_reference_count(){ return r_count_.load(); }
    virtual void dispose() = 0;
    virtual void destroy() = 0;
private:
    std::atomic<int>    r_count_;
};

//used to save the ptr
template <typename Ptr>
class reference_count_ptr : public reference_count_base{
public:
    reference_count_ptr(Ptr p) : p_(p) {}
    virtual void dispose() { delete p_; }
    virtual void destroy() { delete this; }
    reference_count_ptr(const reference_count_ptr&) = delete;
    reference_count_ptr& operator=(const reference_count_ptr&) = delete;
private:
    Ptr        p_;
};

//combine the ptr and count
template <typename Ptr>
class data_block_count{
public:
    data_block_count(Ptr p){
        msg_r_count_ = new reference_count_ptr<Ptr>(p);
    }
    //? other is a const, and other.msg_data_ is a const too?
    //? assign a const to non-const this->msg_r_count_
    data_block_count(const data_block_count& other) : msg_r_count_(other.msg_r_count_){
        if(msg_r_count_ != 0) msg_r_count_->add_reference();
    }
    data_block_count& operator=(const data_block_count& other){
        //? operator=(*this)
        reference_count_base* tmp = other.msg_r_count_;
        if(tmp != msg_r_count_){
            if(tmp != 0){
                tmp->add_reference();
            }
            if(msg_r_count_ != 0){
                msg_r_count_->release();
                msg_r_count_ = tmp;
            }
        }
        return *this;
    }
    ~data_block_count(){
        if(msg_r_count_ != nullptr){
            msg_r_count_->release();
        }
    }
private:
    reference_count_base*   msg_r_count_;
};

template <typename T>
class data_block{
public:
    data_block(T* msg_data, size_t size)
        : msg_data_(msg_data)
        , count_(msg_data)
        , size_(size) {
        if(msg_data_ == 0){
            LOG(WARNING) << "msg_data is nullptr";
            size_ = 0;
        }
    }
    data_block(const data_block& other)
        : msg_data_(other.msg_data_)
        , size_(other.size_){ }
    ~data_block(){}
private:
    T*                          msg_data_;
    data_block_count<T>         count_;
    size_t                      size_;
};


//TODO delete or not delete 
template <typename T, bool need_delete = true>
class message_block{
public:
    message_block(T* base, size_t size) 
        : base_(base) {}
    message_block(const message_block& other) 
        : base_(other.base_) { }
    //if we don't want to copy we have to use a reference count to count the usage
    message_block& operator= (const message_block& other){//here copied the void* data
        // if(this == &other) return *this;
        // base_ = operator new (this->size_);
        // memcpy(this->base_, other.base_, other.size_);
    }
    ~message_block(){
    }
private:
    data_block<T>*              base_;
};


template <typename T, bool need_delete = true>
class message_queue{
public:
    using microseconds = std::chrono::microseconds;
    using mutex_type = std::mutex;
    using guard_type = lock_guard<mutex_type>;
    using cv_type = std::condition_variable;
    using lock_type = std::unique_lock<mutex_type>;
    using message_block_type = message_block<T, need_delete>;
    message_queue(size_t hwm = 1024, size_t lwm = 1024);
    int open();
    int close();
    ~message_queue();
    int enqueue_head(message_block_type* message, const microseconds& timeout);
    int enqueue_tail(message_block_type* message, const microseconds& timeout);
    int dequeue_head(message_block_type*& first_message, const microseconds& timeout);
    int dequeue_tail(message_block_type*& dequeued, const microseconds& timeout);
    bool is_full() const {return deque_ptr_->size() == high_water_mark_;}
    bool is_empty() const {return deque_ptr_->size() == 0;}
    size_t current_message_count() const {return deque_ptr_->size();}

protected:
    int wait_not_empty_condition(const microseconds& timeout);
    int wait_not_full_condition(const microseconds& timeout);
    void signal_enqueue_waiters(){ not_full_cv_.notify_one(); }
    void signal_dequeue_waiters(){ not_empty_cv_.notify_one(); }

private:
    size_t                                          high_water_mark_;
    size_t                                          low_water_mark_;
    mutex_type	                                    mutex_;
    //虽然，这里使用了deque，但是并没有使用deque的双端可进可出的特点，只是使用了一端进一端出的特点
    //若要使用双端特性，则需要加上按照优先级进行enqueue、dequeue
    //TODO add priority
    std::shared_ptr<std::deque<message_block_type*>>                deque_ptr_;
    cv_type	                                                        not_empty_cv_;
    cv_type	                                                        not_full_cv_;

};

template <typename T, bool need_delete>
message_queue<T, need_delete>::message_queue(
    size_t hwm, size_t lwm) 
    : high_water_mark_(hwm)
    , low_water_mark_(lwm)
    , deque_ptr_(new std::deque<message_block<T, need_delete>*>{})
    , mutex_()
    , not_empty_cv_()
    , not_full_cv_(){
}
template <typename T, bool need_delete>
int message_queue<T, need_delete>::open(){

}
template <typename T, bool need_delete>
int message_queue<T, need_delete>::close(){
    deque_ptr_->clear();
}
template <typename T, bool need_delete>
message_queue<T, need_delete>::~message_queue(){ }
template <typename T, bool need_delete>
int message_queue<T, need_delete>::enqueue_head(message_block<T, need_delete>* message, const microseconds& timeout){
    guard_type _{mutex_};
    if(wait_not_full_condition(timeout) != 0)//timeout
        return -1;
    deque_ptr_->push_front(message);
    if(deque_ptr_->size() < high_water_mark_)
    signal_dequeue_waiters();
    return 0;
}
    
template <typename T, bool need_delete>
int message_queue<T, need_delete>::enqueue_tail(message_block<T, need_delete>* message, const microseconds& timeout){
    guard_type _{mutex_};
    if(wait_not_full_condition(timeout) != 0)
        return -1;
    deque_ptr_->push_back(message);
    signal_dequeue_waiters();
    return 0;
}
template <typename T, bool need_delete>
int message_queue<T, need_delete>::dequeue_head(message_block<T, need_delete>*& first_message, const microseconds& timeout){
    if(first_message == nullptr){
        LOG(ERROR) << "first_message is null";
        return -1;
    }
    guard_type _{mutex_};
    int result = 0;
    if(wait_not_empty_condition(timeout) != 0)//timeout
        return -1;
    *first_message = *deque_ptr_->front();
    deque_ptr_->pop_front();
    signal_enqueue_waiters();
    return 0;
}
template <typename T, bool need_delete>
int message_queue<T, need_delete>::dequeue_tail(message_block<T, need_delete>*& dequeued, const microseconds& timeout){
    if(dequeued == nullptr){
        LOG(ERROR) << "dequeued is null";
        return -1;
    }
    guard_type _{mutex_};
    if(wait_not_empty_condition(timeout) != 0)//timeout
        return -1;
    *dequeued = *deque_ptr_->back();
    deque_ptr_->back();
    signal_enqueue_waiters();
    return 0;
}

template <typename T, bool need_delete>
int message_queue<T, need_delete>::wait_not_empty_condition(const microseconds& timeout){
    lock_type u_lock{mutex_, std::adopt_lock_t{}};
    int result = 0;
    while(this->deque_ptr_->size() == 0){
        if(not_empty_cv_.wait_for(u_lock, timeout) == std::cv_status::timeout){
            result = -1;
            break;
        }
    }
    return result;
}

template <typename T, bool need_delete>
int message_queue<T, need_delete>::wait_not_full_condition(const microseconds& timeout){
    lock_type u_lock{mutex_, std::adopt_lock_t{}};
    int result = 0;
    while(this->deque_ptr_->size() >= high_water_mark_){
        if(not_full_cv_.wait_for(u_lock, timeout) == std::cv_status::timeout){
            result = -1;
            break;
        }
    }
    return result;
}

}//namespace thread

#endif // _THREAD_MESSAGE_QUEUE_H_