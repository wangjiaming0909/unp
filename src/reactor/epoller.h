#include <sys/epoll.h>
#include <unordered_map>
#include <memory>

namespace reactor{

class epoller
{
public:
    using epoll_event_ptr_t = std::shared_ptr<struct epoll_event>;
    using handle_event_map_t = std::unordered_map<int, epoll_event_ptr_t>;

    epoller(/* args */);
    ~epoller();
    //new 一个epoll_fd
    int open();
    int close();
    int epoll_wait(epoll_event* ret_events, int max_events, int timeout);
    //如果没有此handle, 增加此handle，如果有，那么在此handle上增加事件, 如果存在此事件，返回-1
    int add(int handle, uint32_t event);
    //如果没有handle，报错，如果有handle，没有事件报错， 如果好几个事件，只删除对应的事件
    int del(int handle, uint32_t event);
    //event 0 表示检查时候有这个handle， 不为0表示检查hadnle以及事件
    bool has(int handle, uint32_t event = 0);

private:
    int                         epoll_fd_;
    handle_event_map_t          handle_event_map_;
};

}//reactor
