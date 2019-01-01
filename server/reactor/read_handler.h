#ifndef _UNP_REACTOR_REAC_HANDLER_H_
#define _UNP_REACTOR_REAC_HANDLER_H_
#include "server/reactor/io_handler.h"

namespace reactor{

template <typename data_type>
class ReadHandler : public io_handler<data_type>{
public:
    using base = io_handler<data_type>;
    using mq_type = typename base::mq_type;
    using micro_seconds = typename base::micro_seconds;
    ReadHandler(Reactor& react, mq_type& messageQueue, thread::thread_pool& threadPool)
        : base(react, messageQueue, threadPool){}
    
    virtual void open() override {
        this->reactor_->register_handler(this, event_handler::READ_EVENT);
    }

    virtual int routine() override {

    }
private:

};
}//namespace reactor

#endif // _UNP_REACTOR_REAC_HANDLER_H_