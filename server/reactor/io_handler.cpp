#include "server/reactor/io_handler.h"

using namespace reactor;

io_handler::io_handler(Reactor* react) : event_handler(react){}

int io_handler::handle_input(int handle){

}
int io_handler::handle_output(int handle){

}
int io_handler::handle_timeout(int handle){

}
int io_handler::handle_close(int handle){

}
int io_handler::handle_signal(int handle){

}
int io_handler::get_handle() const{

}
void io_handler::set_handle(int handle){

}