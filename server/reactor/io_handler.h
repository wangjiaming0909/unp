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
    io_handler(Reactor* react, mq_type* messageQueue, thread::thread_pool* threadPool);
    void open();

   virtual int routine() ;
    
    virtual int handle_input(int handle);
    virtual int handle_output(int handle);
    virtual int handle_timeout(int handle);
    virtual int handle_close(int handle);
    virtual int handle_signal(int handle);
    virtual int get_handle() const{

    }
    virtual void set_handle(int handle){

    }

private:
    net::sock_stream peer_;
};
template <typename data_type>
void io_handler<data_type>::open(){
    
}

template <typename data_type>
io_handler<data_type>::io_handler(
    Reactor* react, 
    mq_type* messageQueue, 
    thread::thread_pool* threadPool)
    : base(react, threadPool, messageQueue){ }

template <typename data_type>
int io_handler<data_type>::handle_input(int handle){

}

template <typename data_type>
int io_handler<data_type>::handle_output(int handle){

}

template <typename data_type>
int io_handler<data_type>::handle_timeout(int ){

}

template <typename data_type>
int io_handler<data_type>::handle_close(int handle){

}

template <typename data_type>
int io_handler<data_type>::handle_signal(int ){ }

template <typename data_type>
int io_handler<data_type>::routine() {

}

}
#endif // _UNP_IO_HANDLER_H_