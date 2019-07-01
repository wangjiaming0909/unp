#ifndef CONNECTION_H
#define CONNECTION_H

#include "reactor/event_handler.h"
#include "net/sock_stream.h"
#include "reactor/buffer.h"
#include "reactor/reactor.h"
#include "util/easylogging++.h"
#include <functional>

namespace reactor
{

//TODO 增加一个字段, 指示此 connection_handler 是否应该被关闭了, 在handle_input 和 handle_output 中检查此字段,
// return -1, 调用 handle_close
class connection_handler : public event_handler
{
public:
    connection_handler(Reactor &reactor);
    //read data from sock_stream into buffer input_buffer_, if input_buffer size is below HIGH WATMARK
    //???如果input_buffer已经达到HIGH WATMARK 或者已经达到buffer的最大大小了怎么办
    //buffer do not have the maximum size, chain has
    //when input_buffer's size >= HIGH_WATMARK, we do not read, wait for next to read,
    //but when using edge trigger(default), handle_input will be invoked immediately
    virtual int handle_input(int) override;
    //write data from output_buffer into sock_stream if output_buffer has data
    //当output_buffer 中的数据都flush了，那么也应该 disable writing
    virtual int handle_output(int) override;
    virtual int handle_timeout(int) noexcept override;
    virtual int handle_close(int) override;
    virtual int handle_signal(int) override;
    virtual int get_handle() const override;
    virtual void set_handle(int) override;
    virtual ~connection_handler() override;

public:
    net::sock_stream &get_sock_stream() { return stream_; }
    //read 只是从intput_buffer中读取数据，并不会从socket中读取
    //return bytes read
    int64_t read(char *data_out, uint32_t data_len);
    //data_len means the length of data_out
    //not the returned length of data_out
    int64_t read_line(char *data_out, uint32_t data_len, buffer_eol_style eol);
    //最初我们不注册 write事件，因为sock_stream一直都是可写，直到达到 socket 出缓冲区的 HIGH WATER MARK 为止
    //所以handle_output会被频繁调用
    //write 仅仅是将data写进output缓冲区的末尾，至于什么时候会被写进socket中，看buffer中有多少数据
    //return bytes written
    int64_t write(const char *data, uint32_t len);
    template <typename T>
    int write(const T &data);

    virtual int open();
    void close();
    void clear_input_buffer()
    {
        input_buffer_.drain(1);
    }

protected:
    virtual int close_read(int) override;
    virtual int close_write(int) override;

    //check input_buffer,
    //1, if input_buffer has enough data
    //2, no data or no enough data(比如想要一行, 但是沒有讀到所給的換行符)
    //(尝试从stream中读取, 是否应该从stream中读取，毕竟这是由reactor的handle_input控制的)
    //如果不检查stream， 那么应该返回什么信息
    //    int read_i();
    //write 应该事先检查write是否enabled了，如果没有，把数据写进buffer，然后，register write 事件到reactor
    //如果enabled了，表明 正在flush 数据到sock_stream, 那么直接把数据添加到buffer中
    //    int write_i();

    void check_and_invoke_close_callback();

public:
    //registering
    int enable_reading();
    //registering
    int enable_writing();
    int disable_reading();
    int disable_writing();
    void set_closed_callback(std::function<void(int)> callback) { closed_callback_ = std::move(callback); }

protected:
    net::sock_stream stream_;
    buffer input_buffer_;
    buffer output_buffer_;
    bool read_enabled_;
    bool write_enabled_;
    static const unsigned int BUFFER_HIGH_WATER_MARK;
    std::function<void(int)> closed_callback_;
};

//template <typename T>
//int connection_handler::write(const T& data)
//{

//}

} // namespace reactor
#endif /* CONNECTION_H */
