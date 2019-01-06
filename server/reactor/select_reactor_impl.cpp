#include "select_reactor_impl.h"
#include "server/util/unp_time.h"

using namespace reactor;

select_demultiplex_table::select_demultiplex_table(size_t size) : event_vector_(){
    if(size > MAX_NUMBER_OF_HANDLE) {
        LOG(ERROR) << "select_demultiplex_table size specified is " 
                   << size << ", bigger than " 
                   << MAX_NUMBER_OF_HANDLE;
        event_vector_.resize(MAX_NUMBER_OF_HANDLE); //resize requires that T has default constructor
    } else event_vector_.resize(size);
}

event_handler* select_demultiplex_table::get_handler(int handle, Event_Type type) const{
    if(!is_handle_in_range(handle)){
        LOG(WARNING) << "handle is not in range, handle: " << handle;
        return 0;
    }
    return event_vector_[handle].get_handler(type);
}

int select_demultiplex_table::bind(int handle, event_handler* handler, Event_Type type){
    // auto event_type_str = event_type_to_string(type);
    // LOG(INFO) << "bind handle: " << handle << " type: " << event_type_str;
    if(!is_valid_handle(handle) || handler == 0) {
        LOG(ERROR) << "handle is not in range or handler is null handle: " 
                    << handle << " handler: " << handler;
        return -1;
    }
    if((int)event_vector_.size() <= handle)
        event_vector_.resize(handle + handle/2);
    event_vector_[handle].handle = handle;
    event_vector_[handle].bind_new(type, handler);
    if(handle > current_max_handle_p_1_ - 1){
        current_max_handle_p_1_ = handle + 1;
    }
    return 0;
}

int select_demultiplex_table::unbind(int handle){
    if(!is_handle_in_range(handle) || event_vector_[handle].event_count == 0){
        LOG(WARNING) << "handle is not in the table or handle has no handler, handle: " << handle;
        return -1;
    }
    //if handle is the max_handle - 1
    int tmp = handle;
    if(current_max_handle_p_1_ == handle + 1){
        while(tmp-- != -1 && event_vector_[tmp].handle == INVALID_HANDLE);
        current_max_handle_p_1_ = tmp + 1;
    }
    event_vector_[handle].handle = INVALID_HANDLE;
    event_vector_[handle].clear();

    return 0;
}

int select_demultiplex_table::unbind(int handle, const event_handler* handler, Event_Type type){
    if(!is_handle_in_range(handle) || event_vector_[handle].get_handler(type) == nullptr){
        LOG(WARNING) << "handle is not in the table or handle has no handler, handle: " << handle;
        return -1;
    }
    int tmp = handle;
    int ret = event_vector_[handle].unbind(type, handler);
    //如果当前handle只有这一个handler存在，那么就需要重新寻找最大的fd，否则不需要
    if(event_vector_[handle].event_count == 0){
        if(handle + 1 == current_max_handle_p_1_){//find the next max handle
            while(tmp-- != INVALID_HANDLE && event_vector_[tmp].handle == INVALID_HANDLE);
            current_max_handle_p_1_ = tmp + 1;
        }
    }
    return ret;
}

bool select_demultiplex_table::is_handle_in_range(int handle) const {
    if(handle < 0 || ((handle + 1) > current_max_handle_p_1_)) 
        return false;
    return true;
}

bool select_demultiplex_table::is_valid_handle(int handle) const {
    if(handle == INVALID_HANDLE || handle > MAX_NUMBER_OF_HANDLE)
        return false;
    return true;
}

const long int select_demultiplex_table::MAX_NUMBER_OF_HANDLE;


void select_reactor_impl::handle_events(std::chrono::microseconds* timeout) {
    int n = 0;
    while((n = this->select(timeout)) >= 0){
        LOG(INFO) << n << " fd ready...";
        dispatch(n);
    }
    LOG(WARNING) << "select returned -1: " << strerror(errno);
}

//if returned value < 0, means error
//if returned value == 0, means ?
//if returned value > 0, means n fd(s) are ready
int select_reactor_impl::select(std::chrono::microseconds* timeout){
    LOG(INFO) << "preparing to select...";
    const int width = this->demux_table_.get_current_max_handle_p_1();
    dispatch_sets_.read_set = this->wait_sets_.read_set;
    dispatch_sets_.write_set = this->wait_sets_.write_set;
    dispatch_sets_.exception_set = this->wait_sets_.exception_set;
    auto timeout_timeval = util::duration_to_timeval<std::chrono::microseconds>(timeout);

//    LOG(INFO) << "trying to wait on " << width << " fds...";
    int read_fd_count = dispatch_sets_.read_set.handles_count();
    int write_fd_count = dispatch_sets_.write_set.handles_count();
    int exception_fd_count = dispatch_sets_.exception_set.handles_count();
    LOG(INFO) << "read fd count: " << read_fd_count;
    LOG(INFO) << "write fd count: " << write_fd_count;
    LOG(INFO) << "exception fd count: " << exception_fd_count;
    if((read_fd_count + write_fd_count + exception_fd_count) == 0){
        LOG(ERROR) << "there is no fd to select";
        return -1;
    }

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
    //TODO what if number_of_active_handles == 0 and not timeout, will this happen
    return number_of_active_handles;
}

int select_reactor_impl::dispatch(int active_handle_count){
    int number_of_handles_dispatched = 0;
    return dispatch_io_handlers(active_handle_count, number_of_handles_dispatched);
}

int select_reactor_impl::register_handler(event_handler* handler, Event_Type type) {
    (void)handler;
    (void)type;
}

int select_reactor_impl::unregister_handler(event_handler *handler, Event_Type type) {
    (void)handler;
    (void)type;
}

int select_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type){
    LOG(INFO) << "registering handler for handle: " << handle << " event: "<< event_type_to_string(type);
    if(handle == INVALID_HANDLE || handler == 0 || type == event_handler::NONE){
        LOG(ERROR) << "handle error or registered type error...";
        return -1;
    }
    switch (type) {
        case event_handler::READ_EVENT:
        case event_handler::ACCEPT_EVENT:
            wait_sets_.read_set.set_bit(handle);
            break;
        case event_handler::WRITE_EVENT:
            wait_sets_.write_set.set_bit(handle);
            break;
        case event_handler::EXCEPT_EVENT:   
            wait_sets_.exception_set.set_bit(handle);
            break;
        default:
            return -1;
    }
    return demux_table_.bind(handle, handler, type);
}

int select_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type){
    LOG(INFO) << "unregistering handler for handle: " << handle << " event: " << event_type_to_string(type);
    if(handle == INVALID_HANDLE || handler == 0 || type == event_handler::NONE){
        LOG(ERROR) << "handle error or registered type error...";
        return -1;
    }
    switch (type) {
        case event_handler::READ_EVENT:
        case event_handler::ACCEPT_EVENT:
            wait_sets_.read_set.unset_bit(handle);
            break;
        case event_handler::WRITE_EVENT:
            wait_sets_.write_set.unset_bit(handle);
            break;
        case event_handler::EXCEPT_EVENT:
            wait_sets_.exception_set.unset_bit(handle);
            break;
        default:
            return -1;
    }
    return demux_table_.unbind(handle, handler, type);
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
    return ret;
}

//
int select_reactor_impl::dispatch_io_set(
        int number_of_active_handles, 
        int& number_of_handles_dispatched,
        event_handler::Event_Type type,//can tell us what we are doing: handling read, write or exception events
        unp::handle_set& dispatch_set,
        unp::handle_set& ready_set,
        HANDLER callback){
    int current_handle = -1;


    //go throuth the handle_set, dispatch all the handles
    while(
        ((current_handle = dispatch_set.next_handle(current_handle)) != INVALID_HANDLE) && 
        (number_of_handles_dispatched < number_of_active_handles))
    {
        LOG(INFO) << "dispatching... handle: " << current_handle << " event: " << event_type_to_string(type);
        ++number_of_handles_dispatched;
        event_handler* handler = this->demux_table_.get_handler(current_handle, type);
        if(handler == 0) return -1;
        int ret = (handler->*callback) (current_handle);

        if(ret == -1){
            //TODO ret handling
            auto event_type_str = event_type_to_string(type);
            LOG(INFO) << "unbinding handle: " << current_handle << " event: " << event_type_str;
            this->demux_table_.unbind(current_handle);
            handler->handle_close(current_handle);
            dispatch_set.unset_bit(current_handle);
            ready_set.unset_bit(current_handle);
        }else{
            LOG(INFO) <<"keep listening on handle: " << current_handle << " event: ";// << event_type_to_string(type);
        }
    }
    return 0;
}
