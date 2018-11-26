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
        LOG(WARNING) << "handle is not in range, handle: " << handle;
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
    dispatch(n);
}

int select_reactor_impl::select(std::chrono::microseconds* timeout){
    const int width = this->demux_table_.get_current_max_handle_p_1();
    dispatch_sets_.read_set = this->wait_sets_.read_set;
    dispatch_sets_.write_set = this->wait_sets_.write_set;
    dispatch_sets_.exception_set = this->wait_sets_.exception_set;
    auto timeout_timeval = util::duration_to_timeval<std::chrono::microseconds>(timeout);
    int number_of_active_handles = ::select(width, 
                     dispatch_sets_.read_set.get_select_fd_set_ptr(),
                     dispatch_sets_.write_set.get_select_fd_set_ptr(),
                     dispatch_sets_.exception_set.get_select_fd_set_ptr(),
                     timeout_timeval.get());
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

int select_reactor_impl::dispatch(int active_handle_count){
    int number_of_handlers_dispatched = 0;
    return dispatch_io_handlers(active_handle_count, number_of_handlers_dispatched);
}

void select_reactor_impl::register_handler(event_handler* handler, Event_Type type) {

}

void select_reactor_impl::unregister_handler(event_handler *handler, Event_Type type) {

}

void select_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type){
    if(handle == INVALID_HANDLE || handler == 0 || type != event_handler::NONE){
        return;
    }
    if(type == event_handler::READ_EVENT){
        wait_sets_.read_set.set_bit(handle);
    }else if(type == event_handler::WRITE_EVENT){
        wait_sets_.write_set.set_bit(handle);
    }else if(type == event_handler::EXCEPT_EVENT){
        wait_sets_.exception_set.set_bit(handle);
    }
    demux_table_.bind(handle, handler, type);
}

void select_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type){

}

//dispatch io_handlers read_set, write_set, exception_set
int select_reactor_impl::dispatch_io_handlers(int active_handle_count, int& io_handles_dispatched){
    int ret = dispatch_io_set(
        active_handle_count, 
        io_handles_dispatched, 
        event_handler::READ_EVENT, 
        this->dispatch_sets_.read_set,
        this->ready_sets_.read_set,
        &event_handler::handle_input);
    //TODO check ret
    ret = dispatch_io_set(
        active_handle_count, 
        io_handles_dispatched, 
        event_handler::WRITE_EVENT, 
        this->dispatch_sets_.write_set,
        this->ready_sets_.write_set,
        &event_handler::handle_output);
    //TODO check ret
    ret = dispatch_io_set(
        active_handle_count, 
        io_handles_dispatched, 
        event_handler::EXCEPT_EVENT, 
        this->dispatch_sets_.exception_set,
        this->ready_sets_.exception_set,
        &event_handler::handle_output);
    //TODO check ret
}

//
int select_reactor_impl::dispatch_io_set(
        int number_of_active_handlers, 
        int& number_of_handlers_dispatched,
        event_handler::Event_Type type,//can tell us what we are doing: handling read, write or exception events
        unp::handle_set& dispatch_set,
        unp::handle_set& ready_set,
        HANDLER callback){
    int current_handle = -1;
    //go throuth the handle_set, dispatch all the handles
    while((current_handle = dispatch_set.next_handle(current_handle)) != INVALID_HANDLE
          && number_of_handlers_dispatched < number_of_active_handlers){
        ++number_of_handlers_dispatched;
        event_handler* handler = this->demux_table_.get_handler(current_handle);
        if(handler == 0) return -1;
        int ret = (handler->*callback) (current_handle);
        //TODO ret handling
        dispatch_set.unset_bit(current_handle);
        ready_set.unset_bit(current_handle);
    }
    return 0;
}