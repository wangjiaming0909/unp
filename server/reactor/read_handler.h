#ifndef _UNP_REACTOR_REAC_HANDLER_H_
#define _UNP_REACTOR_REAC_HANDLER_H_
#include "server/reactor/io_handler.h"
#include <chrono>

using namespace thread;
using namespace std::chrono_literals;
namespace reactor{

//! One ReadHandler can only handle one fd, cause it only has one peer_
//! but there could be any threads that handle this same fd at the same time
template <typename data_type>
class ReadHandler : public io_handler<data_type>{
public:
    using base = io_handler<data_type>;
    using mq_type = typename base::mq_type;
    using micro_seconds = typename base::micro_seconds;

    ReadHandler(Reactor& react, thread::thread_pool& threadPool, mq_type& messageQueue)
        : base(react, threadPool, messageQueue)
    {
        memset(buffer_, 0, 128);
        this->current_event_ = event_handler::READ_EVENT;
    }
    
    virtual int open() override 
    {
        LOG(INFO) << "opening ReadHandler...";
        return this->reactor_->register_handler(this->peer_.get_handle(), this, this->current_event_);
    }

/***    when the handle is ready to read, select or poll invoke handle_input,
 *      here put data into the message_queue, and activate the thread to read the data,
 *      thread routine is routine function
***/
    virtual int handle_input(int handle) 
    {
        if(!this->is_handle_valid(handle)) return -1;
        data_block<data_type> data{new data_type, true};
        this->put_data_and_activate(data);
    }

    virtual int handle_close(int handle) 
    {
        (void)handle;
        this->peer_.close_writer();
//        TODO should read_handle close the fd
//        this->peer_.close_reader();
//        this->peer_.close();
    }

    //! thread safty 存在race condition 当多个线程处理 在 get_data 之后, 并且 得到的是同一份数据时，并且对这份数据做了操作
    //! 会得到同一份数据么???
    virtual int routine() override {
        //need to dequeue，为了使得没有消息时，该线程会被阻塞，如果不dequeue，就会read阻塞
        //虽然没有使用这个data
        LOG(INFO) << "routine has started...";
        //! important 
        //! 由于处理程序可能比reactor 的分派慢,因此会存在很多其实可能时重复的message被加入到queue中
        //! 因此如果当前fd已经关闭就直接不再去get_data
        if(this->peer_.get_handle() == INVALID_HANDLE) {
            return -1;
        }
        data_block<data_type> data{};

        int ret = this->get_data(&data);
        if(ret != 0 ) {
            LOG(INFO) << "get data error";
            return -1;
        }
        //!! 如果不会得到同一份数据, 那么就不需要枷锁了,随意修改这份数据
        { //! lock if you will modify the data
            LOG(INFO) << "data: " << *data;
        } //!
        //! when multi threads read the fd together what will hanppen?
        std::chrono::microseconds timeout = 2s;
        if(this->peer_.read(static_cast<void*>(buffer_), 64, &timeout) < 0){
            LOG(ERROR) << "read none..." << strerror(errno) ;
            return 0;
        }
        LOG(INFO) << "get data from peer: " << buffer_ << " thread_id: " << std::this_thread::get_id();
        if(ret = this->peer_.send(static_cast<void*>(buffer_), strlen(buffer_), 0, 0) < 0)
        {
            LOG(WARNING) << "send error: " << strerror(errno);
            return 0;
        }
        if(buffer_[0] == 'q') {
            LOG(INFO) << "remote sended a quit, closing...";
            return -1;
        }
        return 0;
    }
private:
    char buffer_[128];
};
}//namespace reactor

#endif // _UNP_REACTOR_REAC_HANDLER_H_
