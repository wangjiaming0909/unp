#include "server/reactor/echo_connection_handler.h"

using namespace reactor;

echo_connection_handler::echo_connection_handler(Reactor& reactor) : connection_handler(reactor)
{
}

echo_connection_handler::~echo_connection_handler()
{
}

int echo_connection_handler::handle_input(int handle)
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

    int recv_buf_size = 4096;

    int ret = stream_.read(input_buffer_, recv_buf_size, &timeout);
    if(ret < 0)
    {
        LOG(ERROR) << "Read error: " << strerror(errno);
        return -1;
    }
    if(ret == 0)
    {
        LOG(INFO) << "Read EOF";
        return -1;
    }

    //read {ret} bytes of data, write it to output_buffer
    char* dataRead = static_cast<char*>(::calloc(recv_buf_size, 1));

    this->read(dataRead, ret);
    this->write(dataRead, ret);

    ::free(dataRead);

    return 0;
}