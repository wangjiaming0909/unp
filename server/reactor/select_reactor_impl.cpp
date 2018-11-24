#include "select_reactor_impl.h"

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
    if(table_.size() <= handle)
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


int select_reactor_impl::select(select_reactor_handle_set& dispatch_set, 
    std::chrono::microseconds* timeout){
    
}

int select_reactor_impl::dispatch(){

}