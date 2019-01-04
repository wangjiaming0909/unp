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
    }
    
    virtual void open() override {
        LOG(INFO) << "opening ReadHandler...";
        this->reactor_->register_handler(this->peer_.get_handle(), this, event_handler::READ_EVENT);
    }

    virtual int handle_input(int handle) {
        (void)handle;
        data_block<data_type> data{};
        int ret = this->put_data(&data);
        if(ret == 0)
            this->activate(1);
        return ret;
    }

    virtual int routine() override {
        //need to dequeue，为了使得没有消息时，该线程会被阻塞，如果不dequeue，就会read阻塞
        //虽然没有使用这个data
        LOG(INFO) << "routine";
        this->get_data();
        if(this->peer_.read(static_cast<void*>(buffer_), 64, 0) <= 0){
            LOG(ERROR) << "read none..." ;
            return -1;
        }
        auto id = std::this_thread::get_id();
        LOG(INFO) << buffer_ << " " << id;
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
