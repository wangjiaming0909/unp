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
}

int connection_handler::handle_input(int )
{

}

int connection_handler::handle_output(int )
{

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

}

int connection_handler::enable_writing()
{

}
