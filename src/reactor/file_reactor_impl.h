#pragma once
#include "reactor/reactor_implementation.h"
#include "reactor/poll_demultiplex_table.h"
#include <boost/noncopyable.hpp>
#include <mutex>

namespace reactor
{
class FileReactorImpl : public reactor_implementation, public boost::noncopyable
{
public:
  FileReactorImpl();
  ~FileReactorImpl();
  virtual int handle_events(std::chrono::microseconds *timeout) override;
  virtual int register_handler(EventHandler* handler, Event_Type type) override;
  virtual int register_handler(int handle, EventHandler *handler, Event_Type type) override;
  virtual int unregister_handler(EventHandler *handler, Event_Type type) override;
  virtual int unregister_handler(int handle, EventHandler *handler, Event_Type typl) override;
  virtual bool hasEvent(EventHandler::Event_Type type) const override;

  int dispatch(int active_handle_count);
  int dispatch(int handle, Event_Type type, EventHandler* handler);

private:
    int fd_count_;
    PollEventRepo demux_table_;
    std::mutex demux_table_mutex_;
};
}
