#ifndef CONNECTION_H
#define CONNECTION_H

#include "reactor/EventHandler.h"
#include "net/sock_stream.h"
#include "reactor/buffer.h"
#include "reactor/reactor.h"
#include "util/easylogging++.h"
#include "boost/intrusive/list_hook.hpp"
#include "reactor/IConnection.h"

#include <functional>

namespace reactor
{


using IntrusiveListBaseHook = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
//TODO 增加一个字段, 指示此 connection_handler 是否应该被关闭了, 在handle_input 和 handle_output 中检查此字段,
// return -1, 调用 handle_close
template <typename Conn_T>
class connection_handler : public EventHandler, public Conn_T, public IntrusiveListBaseHook, public IConnection
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
    net::sock_stream &get_sock_stream() override { return stream_; }
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

    virtual int open() override;
    void close() override ;
    void closeStream();
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

template <typename Conn_T>
connection_handler<Conn_T>::connection_handler(Reactor &reactor)
    : EventHandler(reactor)
    , Conn_T(*this)
    , stream_()
    , input_buffer_()
    , output_buffer_()
    , read_enabled_(false)
    , write_enabled_(false)
{
}

template <typename Conn_T>
connection_handler<Conn_T>::~connection_handler()
{
    close();
    closeStream();
}

template <typename Conn_T>
const unsigned int connection_handler<Conn_T>::BUFFER_HIGH_WATER_MARK = 100 * buffer_chain::MAXIMUM_CHAIN_SIZE;

template <typename Conn_T>
int connection_handler<Conn_T>::handle_input(int handle)
{
    if (input_buffer_.total_len() >= connection_handler::BUFFER_HIGH_WATER_MARK)
    {
        return 0;
    }

    if (handle != stream_.get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

    std::chrono::microseconds timeout = 2s;

    int recv_buf_size = 0;
    // socklen_t optlen = sizeof(recv_buf_size);
    /*
    int ret = getsockopt(handle, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, 0);
    if(ret != 0)
    {
        LOG(ERROR) << "Get Rcv_buf size error: " << strerror(errno);
    }
	 */
    recv_buf_size = recv_buf_size == 0 ? 4096 : recv_buf_size;

    int ret = stream_.read(input_buffer_, recv_buf_size, &timeout);
    if (ret < 0)
    {
        LOG(ERROR) << "Read error: " << strerror(errno);
        return 0;
    }
    if (ret == 0)
    {
        LOG(INFO) << "Read EOF";
        return -1;
    }
    return 0;
}

template <typename Conn_T>
int connection_handler<Conn_T>::handle_output(int handle)
{
    if (output_buffer_.buffer_length() == 0)
    {
        return 0;
    }

    if (handle != stream_.get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

    int try_times = 3;

#ifndef DEFAULT_SEND_SIZE
#define DEFAULT_SEND_SIZE 4096
#endif

    for (; try_times > 0; try_times--)
    {
        //行为： 最多pullup 4096 bytes
        size_t pullupSize = DEFAULT_SEND_SIZE > output_buffer_.buffer_length() ? output_buffer_.buffer_length() : DEFAULT_SEND_SIZE;
        auto data_p = output_buffer_.pullup(pullupSize);
        int bytes_send = stream_.send(static_cast<const void *>(data_p), pullupSize, 0);
        if (bytes_send <= 0)
        {
            LOG(ERROR) << "Send error: " << strerror(errno);
            LOG(INFO) << "retrying... " << try_times + 1 << " time";
            continue;
        }

        //socket send buffer could be full, try 3 times, if can't send also, give up
        output_buffer_.drain(bytes_send);
        //although, we give up here
        //if using edge trigger, handle_output will be invoked immediately
        if (output_buffer_.buffer_length() == 0)
        {
            break;
        }
    }

    //if buffer has no data, disabling the writing event
    if (output_buffer_.buffer_length() == 0)
    {
        if (disable_writing() != 0)
        {
            LOG(ERROR) << "Disable writing error: " << strerror(errno);
        }
    }
    return 0;
}

template <typename Conn_T>
int connection_handler<Conn_T>::handle_timeout(int) noexcept
{
    return 0;
}

template <typename Conn_T>
int connection_handler<Conn_T>::handle_close(int)
{
    close();
    return 0;
}

template <typename Conn_T>
int connection_handler<Conn_T>::handle_signal(int)
{
    return 0;
}

template <typename Conn_T>
int connection_handler<Conn_T>::get_handle() const
{
    return stream_.get_handle();
}

template <typename Conn_T>
void connection_handler<Conn_T>::set_handle(int)
{
}

template <typename Conn_T>
int64_t connection_handler<Conn_T>::read(char *data_out, uint32_t data_len)
{
    if (data_out == 0 || data_len == 0)
        return -1;

    if (input_buffer_.buffer_length() == 0)
        return 0;

    uint32_t buf_len = input_buffer_.buffer_length();
    uint32_t len_gonna_pullup = data_len;
    if (buf_len < data_len)
    {
        len_gonna_pullup = buf_len;
    }

    input_buffer_.remove(data_out, len_gonna_pullup);
    return len_gonna_pullup;
}

template <typename Conn_T>
int64_t connection_handler<Conn_T>::read_line(char *data_out, uint32_t data_len, buffer_eol_style eol)
{
    if (data_out == 0 || data_len == 0)
        return -1;

    if (output_buffer_.buffer_length() == 0)
        return 0;

    return input_buffer_.read_line(data_out, data_len, eol);
}

template <typename Conn_T>
int64_t connection_handler<Conn_T>::write(const char *data, uint32_t len)
{
    if (data == 0 || len == 0)
        return -1;

    if (output_buffer_.buffer_length() >= BUFFER_HIGH_WATER_MARK)
    {
        LOG(WARNING) << "Output buffer length got to HIGH_WATER_MARK";
        return 0;
    }

    if (!write_enabled_)
    {
        enable_writing();
    }

    return output_buffer_.append(data, len);
}

template <typename Conn_T>
int connection_handler<Conn_T>::open()
{
    //    if(stream_.get_sock_fd().set_non_blocking() != 0)
    //    {
    //        LOG(ERROR) << "Setting non blocking error: " << strerror(errno);
    //        return -1;
    //    }
    return enable_reading();
}

template <typename Conn_T>
void connection_handler<Conn_T>::close()
{
    if (read_enabled_)
        disable_reading();
    if (write_enabled_)
        disable_writing();
    check_and_invoke_close_callback();
    closeStream();
}

template <typename Conn_T>
void connection_handler<Conn_T>::closeStream()
{
    stream_.close();
}

template <typename Conn_T>
int connection_handler<Conn_T>::close_read(int)
{
    if (read_enabled_)
        disable_reading();
    stream_.close_reader();
    return 0;
}

template <typename Conn_T>
int connection_handler<Conn_T>::close_write(int)
{
    if (write_enabled_)
        disable_writing();
    stream_.close_writer();
    return 0;
}

template <typename Conn_T>
void connection_handler<Conn_T>::check_and_invoke_close_callback()
{
    // if(!read_enabled_ && !write_enabled_)
    // {
    if (closed_callback_)
        closed_callback_(stream_.get_handle());
    // }
}

template <typename Conn_T>
int connection_handler<Conn_T>::enable_reading()
{
    if (read_enabled_ == true)
        return 0;
    read_enabled_ = true;
    return reactor_->register_handler(stream_.get_handle(), this, EventHandler::READ_EVENT);
}

template <typename Conn_T>
int connection_handler<Conn_T>::enable_writing()
{
    if (write_enabled_ == true)
        return 0;
    write_enabled_ = true;
    return reactor_->register_handler(stream_.get_handle(), this, EventHandler::WRITE_EVENT);
}

template <typename Conn_T>
int connection_handler<Conn_T>::disable_reading()
{
    if (read_enabled_ == false)
        return 0;
    read_enabled_ = false;
    int ret = reactor_->unregister_handler(stream_.get_handle(), this, EventHandler::READ_EVENT);
    //    check_and_invoke_close_callback();
    return ret;
}

template <typename Conn_T>
int connection_handler<Conn_T>::disable_writing()
{
    if (write_enabled_ == false)
        return 0;
    write_enabled_ = false;
    int ret = reactor_->unregister_handler(stream_.get_handle(), this, EventHandler::WRITE_EVENT);
    //    check_and_invoke_close_callback();
    return ret;
}

} // namespace reactor
#endif /* CONNECTION_H */
