#include "server/reactor/poll_demultiplex_table.h"

namespace reactor
{


int poll_event_repo::bind_new(Event_Type type, event_handler* handler)
{
    if(type == event_handler::NONE || handler == 0)
    {
        LOG(WARNING) << "can't bind NONE event or handler is nullptr";
        return -1;
    }

    types_and_handlers_.push_back(event_pair{type, handler});
    return 0;
}

int poll_event_repo::unbind(Event_Type type, const event_handler* handler)
{
    if(type == event_handler::NONE || handler == 0)
    {
        LOG(WARNING) << "can't unbind NONE event or handler can't be nullptr";
        return -1;
    }

    auto event_tuple_find = this->find(type);
    if(event_tuple_find == types_and_handlers_.end())
    {
        LOG(WARNING) << "can't unbind, no this type of event";
        return -1;
    }

    if(event_tuple_find->handler != handler)
    {
        LOG(WARNING) << "the handler you unbinded is not the same as you provided";
        return -1;
    }

    types_and_handlers_.erase(event_tuple_find);
    return 0;
}

event_handler* poll_event_repo::get_handler(Event_Type type) const 
{
    auto iter = this->find(type);
    if(iter == types_and_handlers_.end())
    {
        // LOG(WARNING) << "can't unbind, no this type of event";
        return nullptr;
    }
    return iter->handler;
}

std::vector<poll_event_repo::event_tuple>::const_iterator poll_event_repo::find(Event_Type type) const
{
//    size_t size = types_and_handlers_.size();
    return std::find_if(types_and_handlers_.begin(), types_and_handlers_.end(), 
        [type] (const event_tuple& tuple) 
        {
            if((tuple.event == event_handler::READ_EVENT || tuple.event == event_handler::ACCEPT_EVENT) 
                && type == POLLIN)
                return true;
            if((tuple.event == event_handler::WRITE_EVENT || tuple.event == event_handler::CONNECT_EVENT)
                && type == POLLOUT)
                return true;
            if((tuple.event == event_handler::EXCEPT_EVENT && type == POLLPRI)) return true;
			return false;
        });
}

poll_demultiplex_table::poll_demultiplex_table() 
    : table_()
    , size_()
    , mutex_()
    {
    }

event_handler* poll_demultiplex_table::get_handler(int handle, Event_Type type) const 
{
    lock_guard_t guard{mutex_};
    if(table_.size() <= static_cast<size_t>(handle))
        return nullptr;

    return table_[handle].get_handler(type);
}

int poll_demultiplex_table::bind(int handle, event_handler* handler, Event_Type type){
    lock_guard_t guard{mutex_};
    int64_t table_size = static_cast<int64_t>(table_.size());
    if((handle) >= (table_size))
    {
        table_.resize(handle + 4);
    }
    size_++;
    return table_[handle].bind_new(type, handler);
}

//unbind掉绑定到这个handle的所有事件处理器
int poll_demultiplex_table::unbind(int handle){
    lock_guard_t guard{mutex_};
    int handle_count = table_[handle].handle_count();
    if(handle_count <= 0)
    {
        LOG(WARNING) << "No handler bind to handle: " << handle;
        return -1;
    }
    table_[handle].clear();
    size_ -= handle_count;
    return 0;
}

}