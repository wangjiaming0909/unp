#ifndef CONNECTION_H
#define CONNECTION_H

#include "server/reactor/event_handler.h"
#include "server/net/sock_stream.h"
#include "server/reactor/buffer.h"

namespace reactor{


class connection_handler : public event_handler{
public:
    connection_handler(Reactor& reactor);
    //read data from sock_stream into buffer input_buffer_, if input_buffer has space or below HIGH WATMARK
    //???如果input_buffer已经达到HIGH WATMARK 或者已经达到buffer的最大大小了怎么办
    virtual int handle_input(int ) override;
    //write data from output_buffer into sock_stream if output_buffer has data
    //当output_buffer 中的数据都flush了，那么也应该 disable writing
    virtual int handle_output(int ) override;
    virtual int handle_timeout(int ) override;
    virtual int handle_close(int ) override;
    virtual int handle_signal(int ) override;
    virtual int get_handle() const override;
    virtual void set_handle(int ) override;
    virtual ~connection_handler() override;

public:
    net::sock_stream& get_sock_stream() {return stream_;}
    //read 只是从intput_buffer中读取数据，并不会从socket中读取
    //return bytes read
    int read(char* data_out, uint32_t data_len);
    //最初我们不注册 write事件，因为sock_stream一直都是可写，直到达到 socket 出缓冲区的 HIGH WATER MARK 为止
    //所以handle_output会被频繁调用
    //write 仅仅是将data写进output缓冲区的末尾，至于什么时候会被写进socket中，看buffer中有多少数据
    //return bytes written
    int write(const char* data, uint32_t len);
    template <typename T>
    int write(const T& data);

protected:
    int open();
    //close will unregister the events registered, and close the socket too
    int close();
    int close_read();
    int close_write();

    //check input_buffer, 
    //1, if input_buffer has enough data
    //2, no data or no enough data(比如想要一行, 但是沒有讀到所給的換行符)
    //(尝试从stream中读取, 是否应该从stream中读取，毕竟这是由reactor的handle_input控制的)
    //如果不检查stream， 那么应该返回什么信息
    int read_i();
    //write 应该事先检查write是否enabled了，如果没有，把数据写进buffer，然后，register write 事件到reactor
    //如果enabled了，表明 正在flush 数据到sock_stream, 那么直接把数据添加到buffer中
    int write_i();


protected:
    //registering
    int enable_reading();
    //registering
    int enable_writing();
    int disable_reading();
    int disable_writing();
protected:
	net::sock_stream	stream_;
    buffer              input_buffer_;
    buffer              output_buffer_;
    bool 				read_enabled_;
    bool 				write_enabled_;
};


}
#endif /* CONNECTION_H */

