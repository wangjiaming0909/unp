#include "server/reactor/connection_handler.h"

using namespace reactor;

connection_handler::connection_handler(Reactor& reactor)
    : event_handler(reactor)
    , stream_()
    , input_buffer_()
    , output_buffer_()
    , read_enabled_(false)
    , write_enabled_(false)
{
}

connection_handler::~connection_handler()
{
    stream_.close();
}

const unsigned int connection_handler::BUFFER_HIGH_WATER_MARK = 100 * buffer_chain::MAXIMUM_CHAIN_SIZE;

int connection_handler::handle_input(int handle)
{
    if(input_buffer_.total_len() >= connection_handler::BUFFER_HIGH_WATER_MARK)
    {
        return 0;
    }

    if(handle != stream_.get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

    std::chrono::microseconds timeout = 2s;

    int recv_buf_size = 0;
    // socklen_t optlen = sizeof(recv_buf_size);
    int ret = getsockopt(handle, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, 0);
    if(ret != 0)
    {
        LOG(ERROR) << "Get Rcv_buf size error: " << strerror(errno);
    }
    recv_buf_size = recv_buf_size == 0 ? 4096 : recv_buf_size;

    ret = stream_.read(input_buffer_, recv_buf_size, &timeout);
    if(ret < 0)
    {
        LOG(ERROR) << "Read error: " << strerror(errno);
        return 0;
    }
    if(ret == 0)
    {
        LOG(INFO) << "Read EOF";
        return -1;
    }
    return 0;
}

int connection_handler::handle_output(int handle)
{
    if(output_buffer_.buffer_length() == 0)
    {
        return 0;
    }

    if(handle != stream_.get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

    int try_times = 3;

#ifndef DEFAULT_SEND_SIZE
#define DEFAULT_SEND_SIZE 4096
#endif

    for (;try_times > 0; try_times--) {
        auto data_p = output_buffer_.pullup(DEFAULT_SEND_SIZE);
        int bytes_send = stream_.send(static_cast<const void*>(data_p), DEFAULT_SEND_SIZE, 0);
        if(bytes_send <= 0)
        {
            LOG(ERROR) << "Send error: " << strerror(errno);
            return -1;
        }

        //socket send buffer could be full, try 3 times, if can't send also, give up
        output_buffer_.drain(bytes_send);
        //although, we give up here
        //if using edge trigger, handle_output will be invoked immediately
        if(output_buffer_.buffer_length())
        {
            break;
        }
    }

    //if buffer has no data, disabling the writing event
    if(output_buffer_.buffer_length() == 0)
    {
        if(disable_writing() != 0)
        {
            LOG(ERROR) << "Disable writing error: " << strerror(errno);
        }
    }
    return 0;
}

int connection_handler::handle_timeout(int )
{

}

int connection_handler::handle_close(int )
{

}

int connection_handler::handle_signal(int )
{

}

int connection_handler::get_handle() const
{

}

void connection_handler::set_handle(int )
{

}

int connection_handler::open()
{

}

int connection_handler::close()
{

}

int connection_handler::enable_reading()
{
    return reactor_->register_handler(stream_.get_handle(), this, event_handler::READ_EVENT);
}

int connection_handler::enable_writing()
{
    return reactor_->register_handler(stream_.get_handle(), this, event_handler::WRITE_EVENT);
}

int connection_handler::disable_reading()
{
    return reactor_->unregister_handler(stream_.get_handle(), this, event_handler::READ_EVENT);
}

int connection_handler::disable_writing()
{
    return reactor_->unregister_handler(stream_.get_handle(), this, event_handler::WRITE_EVENT);
}
