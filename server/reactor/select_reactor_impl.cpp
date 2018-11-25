#include "select_reactor_impl.h"
#include "../util/unp_time.h"

using namespace reactor;

select_demultiplex_table::select_demultiplex_table(size_t size) : table_(){
    // LOG(TRACE) << "select_demultiplex_table";
    if(size > MAX_NUMBER_OF_HANDLE) {
        LOG(ERROR) << "select_demultiplex_table size specified is size, bigger than " 
                   << MAX_NUMBER_OF_HANDLE;
        table_.resize(MAX_NUMBER_OF_HANDLE);
    } else table_.resize(size);
}
event_handler* select_demultiplex_table::get_handler(int handle) const{
    if(!is_handle_in_range(handle)){
        LOG(ERROR) << "handle is not in range, handle: " << handle;
        return 0;
    }
    return table_[handle].event_handler_;
}
int select_demultiplex_table::bind(int handle, event_handler* handler, Event_Type type){
    if(!is_valid_handle(handle) || handler == 0) {
        LOG(ERROR) << "handle is not in range or handler is null handle: " 
                    << handle << " handler: " << handler;
        return -1;
    }
    if((int)table_.size() <= handle)
        table_.resize(handle + handle/2);
    table_[handle].event_handler_ = handler;
    table_[handle].event_type_ = type;
    table_[handle].handle_ = handle;
    if(handle > current_max_handle_p_1_ - 1){
        current_max_handle_p_1_ = handle + 1;
    }
    return 0;
}
int select_demultiplex_table::bind(const select_event_tuple& event_tuple){
    return bind( event_tuple.handle_, 
                 event_tuple.event_handler_, 
                 event_tuple.event_type_);
}
int select_demultiplex_table::unbind(int handle){
    if(!is_handle_in_range(handle) || table_[handle].event_handler_ == 0){
        LOG(WARNING) << "handle is not in the table, handle: " << handle;
        return -1;
    }
    //if handle is the max_handle - 1
    int tmp = handle;
    if(current_max_handle_p_1_ == handle + 1){
        while(tmp-- != -1 && table_[tmp].handle_ == INVALID_HANDLE);
        current_max_handle_p_1_ = tmp + 1;
    }
    table_[handle].event_handler_ = 0;
    table_[handle].event_type_ = 0;
    table_[handle].handle_ = INVALID_HANDLE;

    return 0;
}

const long int select_demultiplex_table::MAX_NUMBER_OF_HANDLE;


void select_reactor_impl::handle_events(std::chrono::microseconds* timeout) {
    int n = this->select(timeout);
    dispatch();
}

int select_reactor_impl::select(std::chrono::microseconds* timeout){
    const int width = this->demux_table_.get_current_max_handle_p_1();
    dispatch_set_.read_set = this->wait_set_.read_set;
    dispatch_set_.write_set = this->wait_set_.write_set;
    dispatch_set_.exception_set = this->wait_set_.exception_set;
    timeval timeout_timeval = util::duration_to_timeval(*timeout);
    int number_of_active_handles = ::select(width, 
                     dispatch_set_.read_set.get_select_fd_set_ptr(),
                     dispatch_set_.write_set.get_select_fd_set_ptr(),
                     dispatch_set_.exception_set.get_select_fd_set_ptr(),
                     &timeout_timeval);
    //!could be interrupted, restart?
    if(number_of_active_handles < 0){
        return -1;
    }
    if(number_of_active_handles == 0 && timeout && timeout->count() != 0){
        errno = ETIMEDOUT;
        return -1;
    }
    //check the fds
    if(number_of_active_handles > 0){
        return number_of_active_handles;
    }
    //TODO what if number_of_active_handles == 0 and not timeout
    return number_of_active_handles;
}

int select_reactor_impl::dispatch(){

}
void select_reactor_impl::register_handler(event_handler* handler, Event_Type type) {

}
void select_reactor_impl::remove_handler(event_handler *handler, Event_Type type) {

}

//dispatch io_handlers read_set, write_set, exception_set
int select_reactor_impl::dispatch_io_handlers(int active_handle_count, int& io_handles_dispatched){
    int ret = dispatch_io_set(
            active_handle_count, 
            io_handles_dispatched, 
            event_handler::READ_EVENT, 
            &event_handler::handle_input);
    //TODO check ret
    ret = dispatch_io_set(
            active_handle_count, 
            io_handles_dispatched, 
            event_handler::WRITE_EVENT, 
            &event_handler::handle_output);
    //TODO check ret
    ret = dispatch_io_set(
            active_handle_count, 
            io_handles_dispatched, 
            event_handler::EXCEPT_EVENT, 
            &event_handler::handle_output);
    //TODO check ret
}

//
int select_reactor_impl::dispatch_io_set(int number_of_active_handlers, 
                    int& number_of_handlers_dispatched,
                    event_handler::Event_Type type, //read, write or exception
                    HANDLER callback){
    int current_handle = 0;
    while(dispatch_set_.)
}