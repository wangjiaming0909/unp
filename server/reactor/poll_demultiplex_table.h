#include "server/reactor/event_handler.h"

namespace reactor
{

struct event_pair
{
    event_pair() : event(0), handler(nullptr){}
    event_pair(uint32_t event, event_handler* handler)
        : event(event), handler(handler){}
    uint32_t event;
    event_handler* handler;
};

class poll_event_repo {
public:
    // using event_tuple = std::pair<unsigned int, event_handler*>;
    using event_tuple = event_pair;
    using Event_Type = event_handler::Event_Type;
    poll_event_repo() : types_and_handlers_() {}
    ~poll_event_repo() {}
    int bind_new(Event_Type type, event_handler* handler);
    int unbind(Event_Type type, const event_handler* handler);
    void clear() {types_and_handlers_.clear();}
    event_handler* get_handler(Event_Type type) const ;
    int handle_count() const {return types_and_handlers_.size();};

private:
    std::vector<event_tuple>::const_iterator find(Event_Type type) const;
private:
    std::vector<event_tuple> types_and_handlers_;
};

class poll_demultiplex_table {
public:
    using Event_Type = event_handler::Event_Type;
    poll_demultiplex_table() : table_(), size_(0){}
    event_handler *get_handler(int handle, Event_Type type) const;

    bool has_handle(int handle) const 
    {
        return table_.size() > static_cast<size_t>(handle) && table_[handle].handle_count() > 0;
    }

    const std::vector<poll_event_repo>& get_event_vector() const 
    { 
        return table_;
    }

    int bind(int handle, event_handler *handler, Event_Type type);

    //unbind掉绑定到这个handle的所有事件处理器
    int unbind(int handle);
    int unbind(int handle, const event_handler *handler, Event_Type type)
    {
        return table_[handle].unbind(type, handler);
    }
private:
    std::vector<poll_event_repo>    table_;
    int                             size_;
};




    
} // namespace reactor
