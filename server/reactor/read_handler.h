#ifndef _UNP_REACTOR_REAC_HANDLER_H_
#define _UNP_REACTOR_REAC_HANDLER_H_
#include "server/reactor/io_handler.h"

using namespace thread;
namespace reactor{

template <typename data_type>
class ReadHandler : public io_handler<data_type>{
public:
    using base = io_handler<data_type>;
    using mq_type = typename base::mq_type;
    using micro_seconds = typename base::micro_seconds;
    ReadHandler(Reactor& react, mq_type& messageQueue, thread::thread_pool& threadPool)
        : base(react, messageQueue, threadPool){
        memset(buffer_, 0, 128);
        this->current_event_ = event_handler::READ_EVENT;       
    }
    
    virtual void open() override {
        LOG(INFO) << "opening ReadHandler...";
        this->reactor_->register_handler(this->peer_.get_handle(), this, this->current_event_);
    }

    virtual int handle_input(int handle) {
        (void)handle;
        data_block<data_type> data{new data_type, true};
        int ret = this->put_data(data);
        if(ret == 0)
            this->activate(1);
        return ret;
    }

    virtual int routine() override {
        //need to dequeue，为了使得没有消息时，该线程会被阻塞，如果不dequeue，就会read阻塞
        //虽然没有使用这个data
        LOG(INFO) << "routine has started...";
        data_block<data_type> data{};
        int ret = this->get_data(&data);
        LOG(INFO) << "data: " << *data;
        if(ret != 0 ) {
            LOG(INFO) << "get data error";
            return -1;
        }
        if(this->peer_.read(static_cast<void*>(buffer_), 64, 0) <= 0){
            LOG(ERROR) << "read none..." ;
            return -1;
        }
        LOG(INFO) << "get data from peer: " << buffer_ << " thread_id: " << std::this_thread::get_id();
        if(buffer_[0] == 'q') {
            LOG(INFO) << "remote sended a quit, closing...";
            return -1;
        }
        return 0;
    }

    virtual int handle_close(int handle) {
        this->peer_.close();
    }

private:
    char buffer_[128];
};
}//namespace reactor

#endif // _UNP_REACTOR_REAC_HANDLER_H_
