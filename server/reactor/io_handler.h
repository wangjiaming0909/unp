#ifndef _UNP_IO_HANDLER_H_
#define _UNP_IO_HANDLER_H_

#include "reactor.h"
#include "server/thread/task.h"
#include "server/net/sock_stream.h"

namespace reactor{

//1, io_handler is an event_handler
//2, io_handler is used to handle sock_stream events
    //like read/write from/to sock_stream
    //io_handler will be created by acceptor
//3, io_handler should be a task ==> means that io_handler should inheret from class task
    //means that io_handler will handle the event in a new thread
template <typename data_type>
class io_handler : public thread::task<data_type>{
public:
    using base = thread::task<data_type>;
    using mq_type = typename base::mq_type;
    using micro_seconds = typename base::micro_seconds;

    io_handler(Reactor& react, thread::thread_pool& threadPool, mq_type& messageQueue)
        : base(react, threadPool, messageQueue) , peer_() { }

    net::sock_stream& get_sock_stream() {return peer_;}
    
    // virtual int handle_input(int handle);
    // virtual int handle_output(int handle);
    // virtual int handle_timeout(int handle);

    //? handle_close is invoked by the reactor thread
    virtual int handle_close(int handle) 
    {
        (void)handle;
        this->peer_.close_writer();
        this->peer_.close_reader();
        this->peer_.close();
    }

    // virtual int handle_signal(int handle);
    virtual int get_handle() const { return this->peer_.get_handle(); }
    // virtual void set_handle(int handle){ }
    bool is_handle_valid(int handle){
        if(handle != peer_.get_handle() || peer_.get_handle() == INVALID_HANDLE) {
            // LOG(ERROR) << "INVALID handle specified: " << handle << " existed: " << peer_.get_handle();
            return false;
        }
        return true;
    }

protected:
    net::sock_stream            peer_;
};
}
#endif // _UNP_IO_HANDLER_H_
