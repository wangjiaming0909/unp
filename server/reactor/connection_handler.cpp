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

int connection_handler::handle_input(int handle)
{
    if(handle != stream_.get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

    std::chrono::microseconds timeout = 2s;
    stream_.read()
}

int connection_handler::handle_output(int handle)
{
    if(handle != stream_.get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

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