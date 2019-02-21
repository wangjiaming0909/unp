#ifndef UNP_READ_WRITE_HANDLER_H_
#define UNP_READ_WRITE_HANDLER_H_

#include "server/reactor/io_handler.h"

namespace reactor{

template <typename DataType>
class read_write_handler : public io_handler<DataType>
{
public:
    using base = io_handler<DataType>;
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
        int ret = this->reactor_->register_handler(this->peer_.get_handle(), this, event_handler::WRITE_EVENT);
        if(ret != 0) return -1;
        // ret = this->reactor_->register_handler(this->peer_.get_handle(), this, event_handler::READ_EVENT);
        // if(ret != 0) return -1;
    }

    virtual int handle_input(int handle) override
    {
        if(!this->is_handle_valid(handle)) return -1;
        data_block<DataType> data{new DataType, true};
        return this->put_data_and_activate(data);
    }

    virtual int handle_close(int handle) override 
    {
        (void)handle;
        this->peer_.close_writer();
        this->peer_.close_reader();
        this->peer_.close();
    }

    virtual int handle_output(int handle) override
    {
        if(!this->is_handle_valid(handle)) return -1;
        data_block<DataType> data{new DataType, true};
        this->put_data_and_activate(data);
        return -1;
    }

    virtual int routine() override 
    {
        LOG(INFO) << "routine has started...";
        //! important 
        //! 由于处理程序可能比reactor 的分派慢,因此会存在很多其实可能时重复的message被加入到queue中
        //* 当使用水平触发时,会有很多的相同事件被分派
        //* 使用边缘触发时,则不会
        //! 因此如果当前fd已经关闭就直接不再去get_data
        if(this->peer_.get_handle() == INVALID_HANDLE) {
            return -1;
        }
        data_block<DataType> data{};

        int ret = this->get_data(&data);
        if(ret != 0 ) {
            LOG(INFO) << "get data error";
            return -1;
        }
        //? 如果不会对相同事件进行多次分配,是否就不会存在竞争了?
        //?????????????????????????????????????????????????????
        //!! 如果不会得到同一份数据, 那么就不需要枷锁了,随意修改这份数据
        { //! lock if you will modify the data
            LOG(INFO) << "data: " << *data;
        } //!
        //?????????????????????????????????????????????????????
        if(this->peer_.send(static_cast<void*>(buffer_), 64, 0) <= 0){
            LOG(ERROR) << "send none..." ;
            return -1;
        }
        LOG(INFO) << "sending data to peer: " << buffer_ << " thread_id: " << std::this_thread::get_id();
        return 0;
    }

    ~read_write_handler(){

    }

private:
    char buffer_[128];
};

}//reactor

#endif // UNP_READ_WRITE_HANDLER_H_