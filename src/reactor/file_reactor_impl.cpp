#include "reactor/file_reactor_impl.h"
#include "net/unp.h"
#include <mutex>

namespace reactor
{

FileReactorImpl::FileReactorImpl()
  : reactor_implementation()
  , fd_count_(0)
  , demux_table_()
  , demux_table_mutex_()
{
}

FileReactorImpl::~FileReactorImpl()
{
}

int FileReactorImpl::register_handler(int handle, EventHandler *handler, Event_Type type)
{
  int ret = 0;
  std::lock_guard<std::mutex> guard(demux_table_mutex_);
  if(handle == INVALID_HANDLE || handler == 0 || type == EventHandler::NONE){
    LOG(ERROR) << "Handle error or registered type error...";
    return -1;
  }

  if(demux_table_.getHandler(handle, type) != 0)
  {
    LOG(WARNING) << "Already existed in the demultiplex table, handle: "
      << handle << " event: " << event_type_to_string(type);
    return -1;
  }

  ret = this->demux_table_.bindNew(handle, type, handler);
  if(ret == 0)
    fd_count_++;
  return ret;
}

int FileReactorImpl::unregister_handler(int handle, EventHandler *handler, Event_Type type)
{
  int ret = 0;
  std::lock_guard<std::mutex> guard(demux_table_mutex_);
  if(handle == INVALID_HANDLE || handler == 0 || type == EventHandler::NONE){
    LOG(ERROR) << "Handle error or unregistered type error...";
    return -1;
  }
  if(demux_table_.getHandler(handle, type) == 0)
  {
    LOG(WARNING) << "Can't unregister, didn't find the handle: "
      << handle << " event: " << event_type_to_string(type);
    return -1;
  }

  ret = this->demux_table_.unbind(handle, type, handler);
  if(ret == 0)
    fd_count_--;
  return ret;
}

int FileReactorImpl::dispatch(int active_handle_count)
{
  int handles_dispatched = 0;
  for(auto handle : demux_table_.getHandleSet()) {
    auto handlers = demux_table_.getHandlers(handle);
    for(auto& pair : handlers) {
      int r = dispatch(handle, pair.first, pair.second);
      handles_dispatched++;
      if (r < 0 && pair.second != nullptr) {
        pair.second->handle_close(handle);
      }
    }
  }
  return active_handle_count - handles_dispatched;
}

int FileReactorImpl::dispatch(int handle, Event_Type type, EventHandler* handler)
{
  int ret = 0;
  if (handler == nullptr) {
    LOG(WARNING) << "Handler is a nullptr handle: " << handle << " EventType: " << event_type_to_string(type);
    return -1;
  }
  switch (type) {
    case EventHandler::READ_EVENT:
      ret = handler->handle_input(handle);
      break;
    case EventHandler::WRITE_EVENT:
      ret = handler->handle_output(handle);
      break;
    default:
      LOG(WARNING) << "Error event type in File Reactor Impl: " << event_type_to_string(type);
      ret = -1;
      break;
  }
  return ret;
}

int FileReactorImpl::handle_events(std::chrono::microseconds *timeout)
{
  NOT_USED(timeout);
  if (demux_table_.getHandleSet().size() == 0) {
    LOG(WARNING) << "No events to wait, exiting...";
    return -1;
  }
  return dispatch(fd_count_);
}
int FileReactorImpl::register_handler(EventHandler* handler, Event_Type type)
{
  NOT_USED(handler);
  NOT_USED(type);
  THROW_NOT_IMPLETED_EXCEPTION;
  return 0;
}

int FileReactorImpl::unregister_handler(EventHandler *handler, Event_Type type)
{
  NOT_USED(handler);
  NOT_USED(type);
  THROW_NOT_IMPLETED_EXCEPTION;
  return 0;
}
bool FileReactorImpl::hasEvent(EventHandler::Event_Type) const
{
  return false;
}

}
