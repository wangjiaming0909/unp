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
	/*
    int ret = getsockopt(handle, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, 0);
    if(ret != 0)
    {
        LOG(ERROR) << "Get Rcv_buf size error: " << strerror(errno);
    }
	 */
    recv_buf_size = recv_buf_size == 0 ? 4096 : recv_buf_size;

    int ret = stream_.read(input_buffer_, recv_buf_size, &timeout);
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
        //行为： 最多pullup 4096 bytes
        size_t pullupSize = DEFAULT_SEND_SIZE > output_buffer_.buffer_length() ? output_buffer_.buffer_length() : DEFAULT_SEND_SIZE;
        auto data_p = output_buffer_.pullup(pullupSize);
        int bytes_send = stream_.send(static_cast<const void*>(data_p), pullupSize, 0);
        if(bytes_send <= 0)
        {
            LOG(ERROR) << "Send error: " << strerror(errno);
            LOG(INFO) << "retrying... " << try_times + 1 << " time";
            continue;
        }

        //socket send buffer could be full, try 3 times, if can't send also, give up
        output_buffer_.drain(bytes_send);
        //although, we give up here
        //if using edge trigger, handle_output will be invoked immediately
        if(output_buffer_.buffer_length() == 0)
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
	return 0;
}

int connection_handler::handle_close(int)
{
	close();
	return 0;
}

int connection_handler::handle_signal(int )
{
	return 0;
}

int connection_handler::get_handle() const
{
    return stream_.get_handle();
}

void connection_handler::set_handle(int )
{
}

int64_t connection_handler::read(char* data_out, uint32_t data_len)
{
    if(data_out == 0 || data_len == 0) return -1;

    if(input_buffer_.buffer_length() == 0) return 0;

    uint32_t buf_len = input_buffer_.buffer_length();
    uint32_t len_gonna_pullup = data_len;
    if(buf_len < data_len)
    {
        len_gonna_pullup = buf_len;
    }

    input_buffer_.remove(data_out, len_gonna_pullup);
    return len_gonna_pullup;
}

int64_t connection_handler::read_line(char* data_out, uint32_t data_len, buffer_eol_style eol)
{
    if(data_out == 0 || data_len == 0) return -1;

    if(output_buffer_.buffer_length() == 0) return 0;
    
	return input_buffer_.read_line(data_out, data_len, eol);
}

int64_t connection_handler::write(const char* data, uint32_t len)
{
    if(data == 0 || len == 0) return -1;

    if(output_buffer_.buffer_length() >= BUFFER_HIGH_WATER_MARK)
    {
        LOG(WARNING) << "Output buffer length got to HIGH_WATER_MARK";
        return 0;
    }

    if(!write_enabled_)
    {
        enable_writing();
    }

    return output_buffer_.append(data, len);
}

int connection_handler::open()
{
//    if(stream_.get_sock_fd().set_non_blocking() != 0)
//    {
//        LOG(ERROR) << "Setting non blocking error: " << strerror(errno);
//        return -1;
//    }
    return enable_reading();
}

void connection_handler::close()
{
	if(read_enabled_) disable_reading();
	if(write_enabled_) disable_writing();
    check_and_invoke_close_callback();
    // stream_.close();
}

int connection_handler::close_read(int)
{
	if(read_enabled_) disable_reading();
    stream_.close_reader();
    return 0;
}

int connection_handler::close_write(int)
{
    if(write_enabled_) disable_writing();
    stream_.close_writer();
    return 0;
}

void connection_handler::check_and_invoke_close_callback()
{
    // if(!read_enabled_ && !write_enabled_)
    // {
        if(closed_callback_)
            closed_callback_(stream_.get_handle());
    // }
}

int connection_handler::enable_reading()
{
    if(read_enabled_ == true) return 0;
    read_enabled_ = true;
    return reactor_->register_handler(stream_.get_handle(), this, event_handler::READ_EVENT);
}

int connection_handler::enable_writing()
{
    if(write_enabled_ == true) return 0;
    write_enabled_ = true;
    return reactor_->register_handler(stream_.get_handle(), this, event_handler::WRITE_EVENT);
}

int connection_handler::disable_reading()
{
    if(read_enabled_ == false) return 0;
    read_enabled_ = false;
    int ret = reactor_->unregister_handler(stream_.get_handle(), this, event_handler::READ_EVENT);
//    check_and_invoke_close_callback();
    return ret;
}

int connection_handler::disable_writing()
{
    if(write_enabled_ == false) return 0;
    write_enabled_ = false;
    int ret = reactor_->unregister_handler(stream_.get_handle(), this, event_handler::WRITE_EVENT);
//    check_and_invoke_close_callback();
    return ret;
}
