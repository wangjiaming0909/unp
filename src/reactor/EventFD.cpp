#include <sys/eventfd.h>
#include "EventFD.h"
#include "util/easylogging++.h"
#include "reactor/reactor.h"
#include "reactor/sock_connection_handler.h"


using namespace reactor;

EventFD::EventFD() {
	event_fd_ = ::eventfd(0, EFD_NONBLOCK);
	if(event_fd_ < 0)
	{
		LOG(ERROR) << "eventfd error" << strerror(errno);
		event_fd_ = -1;
	}
}

EventFD::~EventFD() { 
	if(event_fd_ > 0) 
	{
		handler_->close();
		// ::close(event_fd_);
	}
}

void EventFD::wakeup()
{
	if(!handler_) return;
	handler_->clear_input_buffer();
	uint64_t one = 1;
	handler_->get_sock_stream().write(&one, sizeof(one));
//	handler_->write(reinterpret_cast<const char*>(&one), sizeof(one));
}

int EventFD::registerInto(Reactor& reactor)
{
	if(event_fd_ < 0) return -1;
	handler_ = std::make_shared<sock_connection_handler>(reactor);
	handler_->get_sock_stream().setSockFD(event_fd_);
	return handler_->enable_reading();
}

void EventFD::unregister()
{
	if(event_fd_ < 0) return;
	handler_->close();
}