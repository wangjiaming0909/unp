#include "EventFD.h"
#include "server/util/easylogging++.h"
#include <sys/eventfd.h>
#include <future>
#include "server/reactor/reactor.h"
#include "server/reactor/connection_handler.h"


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
		::close(event_fd_);
	}
}

void EventFD::wakeup()
{
	handler_->clear_input_buffer();
	uint64_t one = 1;
	handler_->write(reinterpret_cast<const char*>(&one), sizeof(one));
}

int EventFD::registerInto(Reactor& reactor)
{
	if(event_fd_ < 0) return -1;
	handler_ = std::make_shared<connection_handler>(reactor);
	handler_->get_sock_stream().set_handle(event_fd_);
	return handler_->enable_reading();
}

void EventFD::unregister()
{
	if(event_fd_ < 0) return;
	handler_->close();
}