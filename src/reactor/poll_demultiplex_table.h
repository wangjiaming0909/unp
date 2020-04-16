#ifndef POLL_DEMULTIPLEX_TABLE_H_
#define POLL_DEMULTIPLEX_TABLE_H_

#include "reactor/EventHandler.h"
#include <mutex>
#include <set>

namespace reactor
{

struct event_pair
{
    event_pair() : event(0), handler(nullptr){}
    event_pair(uint32_t event, EventHandler* handler)
        : event(event), handler(handler){}
    uint32_t event;
    EventHandler* handler;
};

class poll_event_repo {
public:
    using event_tuple = event_pair;
    using Event_Type = EventHandler::Event_Type;
    poll_event_repo() : types_and_handlers_() {}
    ~poll_event_repo() {}

    int bind_new(Event_Type type, EventHandler* handler);
    int unbind(Event_Type type, const EventHandler* handler);
    void clear() {types_and_handlers_.clear();}
    EventHandler* get_handler(Event_Type type) const ;
    int handle_count() const {return types_and_handlers_.size();}

private:
    std::vector<event_tuple>::const_iterator find(Event_Type type) const;
private:
    std::vector<event_tuple> types_and_handlers_;
};

struct PollEventRepo
{
public:
    using EventType = EventHandler::Event_Type;
    using Mutex = std::mutex;
    using Guard = std::lock_guard<Mutex>;
    PollEventRepo() = default;
    ~PollEventRepo() = default;

public:
    int bindNew(int handle, EventType event, EventHandler* handler);
    int unbind(int handle, EventType event, const EventHandler* handler);
    int unbind(int handle);
    EventHandler* getHandler(int handle, EventType type);

    bool hasHandle(int handle) const 
    {
        Guard guard{mutex_};
        return handleSet_.count(handle) > 0; 
    }
    uint32_t handleCount() const 
    {
        Guard guard{mutex_};
        return handleSet_.size(); 
    }
    void clear() 
    { 
        Guard guard{mutex_};
        eventsTable_.clear(); 
        handleSet_.clear(); 
    }
private:
    std::set<int>                                     handleSet_;
    std::vector<std::map<EventType, EventHandler*>>   eventsTable_;
    mutable Mutex                                     mutex_;
};

class poll_demultiplex_table {
public:
    using Event_Type = EventHandler::Event_Type;
    using mutex_t = std::mutex;
    using lock_guard_t = std::lock_guard<mutex_t>;

    poll_demultiplex_table();
    EventHandler *getHandler(int handle, Event_Type type) const;

    bool hasHandle(int handle) const 
    {
        lock_guard_t guard{mutex_};
        return table_.size() > static_cast<size_t>(handle) && table_[handle].handle_count() > 0;
    }

    const std::vector<poll_event_repo>& get_event_vector() const 
    { 
        lock_guard_t guard{mutex_};
        return table_;
    }

    int bindNew(int handle, Event_Type type, EventHandler *handler);

    //unbind掉绑定到这个handle的所有事件处理器
    int unbind(int handle);
    int unbind(int handle, Event_Type type, const EventHandler *handler)
    {
        lock_guard_t guard{mutex_};
        size_--;
        return table_[handle].unbind(type, handler);
    }
private:
    std::vector<poll_event_repo>        table_;
    int                                 size_;
    mutable mutex_t                     mutex_;

};

} // namespace reactor
#endif // POLL_DEMULTIPLEX_TABLE_H_
