#include "reactor/poll_demultiplex_table.h"

namespace reactor
{


int PollEventRepo::bindNew(int handle, EventType event, EventHandler* handler) 
{
    if (event == EventHandler::ACCEPT_EVENT) 
    {
        event = EventHandler::READ_EVENT;
    }
    Guard guard{mutex_};
    if (event == EventHandler::NONE || handler == nullptr) 
    {
        LOG(WARNING) << "can't bind NONE event or handler is nullptr";
        return -1;
    }
    if (event == EventHandler::TIMEOUT_EVENT) 
    {
        auto* h = dynamic_cast<TimeoutHandler*>(handler);
        if (h == nullptr)
        {
            LOG(WARNING) << "binding a none timeout handler to a timeout handler";
            return -1;
        }
        auto t = h->expirationTimePoint();
        if (timeoutHandlers_[t].count(h) == 1)
        {
            LOG(WARNING) << "already binded this handler...";
            return -1;
        }
        timeoutHandlers_[t].insert(h);
        return 0;
    }

    if (handle == INVALID_HANDLE) 
    {
        LOG(WARNING) << "can't bind INVALID_HANDLE to none timeout handler";
        return -1;
    }

    handleSet_.insert(handle);
    auto h = static_cast<uint32_t>(handle);
    if (eventsTable_.size() <= h) eventsTable_.resize(h + 1);
    eventsTable_[h][event] = handler;
    return 0;
}

int PollEventRepo::unbind(int handle, EventType event, const EventHandler* handler)
{
    Guard guard{mutex_};
    if (event == EventHandler::NONE || handler == nullptr)
    {
        LOG(WARNING) << "can't unbind NONE event or handler is nullptr";
        return -1;
    }
    
    if (event == EventHandler::TIMEOUT_EVENT) 
    {
        const auto* h = dynamic_cast<const TimeoutHandler*>(handler);
        if (h == nullptr)
        {
            LOG(WARNING) << "binding a none timeout handler to a timeout handler";
            return -1;
        }
        auto t = h->expirationTimePoint();
        if (timeoutHandlers_.count(t) == 0)
        {
            LOG(WARNING) << "no this timeouthandler with timeout expirationTimePoint is: " << t.time_since_epoch().count();
            return -1;
        }
        if (timeoutHandlers_[t].count(const_cast<TimeoutHandler*>(h)) == 0)
        {
            LOG(WARNING) << "can't unbind, no this timeoutHandler registered: " << t.time_since_epoch().count();
            return -1;
        }
        timeoutHandlers_[t].erase(const_cast<TimeoutHandler*>(h));
        return 0;
    }

    if (handleSet_.count(handle) == 0) 
    {
        LOG(WARNING) << "unbinding a noexisted handle: " << handle << " event: " << event;
        return -1;
    }
    if (eventsTable_[handle].count(event) == 0)
    {
        LOG(WARNING) << "unbinding a noexisted event of handle: " << handle << " event: " << event;
        return -1;
    }
    if (eventsTable_[handle][event] != handler)
    {
        LOG(WARNING) << "unbinding a wrong handler, handle: " << handle << " event: " << event;
        return -1;
    }
    handleSet_.erase(handle);
    eventsTable_[handle].erase(event);
    return 0;
}

int PollEventRepo::unbindTimeoutHandlers(const TimePoint_T& tp)
{
    if (timeoutHandlers_.count(tp) == 0)
    {
        LOG(WARNING) << "no this timeout handler at time point: " << tp.time_since_epoch().count();
        return -1;
    }
    timeoutHandlers_.erase(tp);
    return 0;
}

EventHandler* PollEventRepo::getHandler(int handle, EventType type)
{
    Guard guard{mutex_};

    if (handle == INVALID_HANDLE || type == EventHandler::NONE)
    {
        LOG(WARNING) << "can't get handler with INVALID_HANDLE or NONE event";
        return nullptr;
    }

    uint32_t h = static_cast<uint32_t>(handle);
    if (eventsTable_.size() <= h || eventsTable_[h].count(type) == 0)
        return nullptr;
    return eventsTable_[h][type];
}

std::set<TimeoutHandler*> PollEventRepo::getLatestTimeoutHandlers()
{
    if (!hasTimeoutHandler())
    {
        return std::set<TimeoutHandler*>{};
    }
    return timeoutHandlers_.begin()->second;
}

int PollEventRepo::unbind(int handle)
{
    Guard guard{mutex_};
    if (handle == INVALID_HANDLE)
    {
        LOG(WARNING) << "can't unbind INVALID_HANDLE or NONE event or handler is nullptr";
        return -1;
    }
    auto h = static_cast<uint32_t>(handle);
    if (eventsTable_.size() <= h || eventsTable_[h].size() == 0)
    {
        LOG(WARNING) << "can't unbind, no this handle or no events of this handle: " << handle;
        return -1;
    }
    handleSet_.clear(); 
    eventsTable_[h].clear();
    return 0;
}

int poll_event_repo::bind_new(Event_Type type, EventHandler* handler)
{
    if(type == EventHandler::NONE || handler == 0)
    {
        LOG(WARNING) << "can't bind NONE event or handler is nullptr";
        return -1;
    }

    types_and_handlers_.push_back(event_pair{type, handler});
    return 0;
}

int poll_event_repo::unbind(Event_Type type, const EventHandler* handler)
{
    if(type == EventHandler::NONE || handler == 0)
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

EventHandler* poll_event_repo::get_handler(Event_Type type) const 
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
            if((tuple.event == EventHandler::READ_EVENT || tuple.event == EventHandler::ACCEPT_EVENT) 
                && type == POLLIN)
                return true;
            if((tuple.event == EventHandler::WRITE_EVENT || tuple.event == EventHandler::CONNECT_EVENT)
                && type == POLLOUT)
                return true;
            if((tuple.event == EventHandler::EXCEPT_EVENT && type == POLLPRI)) return true;
			return false;
        });
}

poll_demultiplex_table::poll_demultiplex_table() 
    : table_()
    , size_()
    , mutex_()
{ }

EventHandler* poll_demultiplex_table::getHandler(int handle, Event_Type type) const 
{
    lock_guard_t guard{mutex_};
    if(table_.size() <= static_cast<size_t>(handle))
        return nullptr;

    return table_[handle].get_handler(type);
}

int poll_demultiplex_table::bindNew(int handle, Event_Type type, EventHandler* handler){
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
