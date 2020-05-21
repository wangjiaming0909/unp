#ifndef EVENTFD_H
#define EVENTFD_H
#include <memory>

namespace reactor{

class Reactor;
class EventHandler;
class sock_connection_handler;

class EventFD {
public:
    EventFD();
    EventFD(const EventFD& orig) = delete;
	EventFD& operator=(const EventFD& orig) = delete;
    ~EventFD();

	int getEventFD() const {return event_fd_;}
	int registerInto(Reactor& reactor);
	void unregister();
	void wakeup();
private:
	int event_fd_;
	std::shared_ptr<sock_connection_handler> handler_;
};

}
#endif /* EVENTFD_H */

