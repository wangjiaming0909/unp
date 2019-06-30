#ifndef _UNP_WRITE_HANDLER_H_
#define _UNP_WRITE_HANDLER_H_
#include "reactor/io_handler.h"
#include <vector>

namespace reactor
{
template <typename data_type>
class WriteHandler : public io_handler<data_type>
{
public:
    using base = io_handler<data_type>;
    using mq_type = typename base::mq_type;
    using micro_seconds = typename base::micro_seconds;

    WriteHandler(Reactor& react, thread::thread_pool& pool, mq_type& mq)
        : base(react, pool, mq), buffer_()
    {
        this->current_event_ = event_handler::WRITE_EVENT;
    }

    virtual ~WriteHandler()
    {
    }

    virtual int open() override 
    {
        LOG(INFO) << "opening WriteHandler...";
        return this->reactor_->register_handler
            (this->peer_.get_handle(), this, this->current_event_);
    }

    virtual int handle_output(int handle) 
    {
        if(!this->is_handle_valid(handle)) return -1;

    }

    virtual int routine() override 
    {

    }

private:
    std::vector<char> buffer_;
};

} // reactor


#endif // _UNP_WRITE_HANDLER_H_