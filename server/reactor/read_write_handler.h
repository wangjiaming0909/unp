#ifndef UNP_READ_WRITE_HANDLER_H_
#define UNP_READ_WRITE_HANDLER_H_

#include "server/reactor/io_handler.h"

namespace reactor{

template <typename DataType>
class read_write_handler : public io_handler<DataType>
{
public:
    using base = io_handler<data_type>;
    using mq_type = typename base::mq_type;
    using micro_seconds = typename base::micro_seconds;

    read_write_handler(Reactor& react, thread::thread_pool& pool, mq_type& mq)
        : base(react, pool, mq)
    {
        memset(buffer_, 0, 128);
        this->current_event_ = event_handler::READ_EVENT;
    }

    virtual int open() override {
        LOG(INFO) << "register read_write_handler to reactor...";
        int ret = this->reactor->register_handler(this->peer.get_handle(), this, event_handler::WRITE_EVENT)
        if(ret != 0) return -1;
        ret = this->reactor_->register_handler(this->peer_.get_handle(), this, event_handler::READ_EVENT);
        if(ret != 0) return -1;
    }

    virtual int handle_input(int handle) override
    {
        if(!this->is_handle_valid(handle)) return -1;
        data_block<data_type> data{new data_type, true};
        return this->put_data_and_activate(data);
    }

    virtual int handle_output(int handle) override
    {
        if(!this->is_handle_valid(handle)) return -1;
        data_block<data_type> data{new data_type, true};
        return this->put
    }

    virtual int routine() override 
    {

    }

    ~read_write_handler(){

    }

private:
    char buffer_[128];
};

}//reactor

#endif // UNP_READ_WRITE_HANDLER_H_