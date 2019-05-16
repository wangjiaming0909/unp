#include "server/reactor/echo_connection_handler.h"
#include <iostream>
#include <chrono>
#include "server/util/unp_time.h"

using namespace reactor;

// int echo_connection_handler::i = 0;

echo_connection_handler::echo_connection_handler(Reactor& reactor) : connection_handler(reactor)
{
    // util::string time = util::Time::now().toString();
    // std::cout << "constructing echo connectin handler " << i << std::endl;
    // i++;
}

echo_connection_handler::~echo_connection_handler()
{
    LOG(INFO) << "closing a connection handle: " << this->get_handle();
    // util::string time = util::Time::now().toString();
    // std::cout << "destructing echo connection handler "<< i << std::endl;
    // i--;
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