#include "reactor/epoll_reactor_impl.h"
#include "util//easylogging++.h"
#include <chrono>

namespace reactor
{

epoll_reactor_impl::epoll_reactor_impl()
  : fd_count_(0)
  , epoll_fd_(-1)
  , cur_event_()
  , ret_events_()
  , demux_table_()
  , mutex_()
  , epoller_()
{
  this->open();
}

epoll_reactor_impl::~epoll_reactor_impl()
{
  epoller_.close();
}

int epoll_reactor_impl::open()
{
  return epoller_.open();
}

int epoll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{
  using namespace std::chrono;
  int n = 0;
  bool onlyTimeoutEvent = false;
  cached_now_ = steady_clock::now();
  if(timeout == 0 && !demux_table_.hasTimeoutHandler()) {
    n = this->epoll_wait(-1);
  } else {
    using TP = epoll_demultiplex_table::TimePoint_T;
    TP::duration dura_of_timer = TP::duration::max();
    if (demux_table_.hasTimeoutHandler())
      dura_of_timer = demux_table_.getLastestTimeoutPoint() - cached_now_;
    TP::duration nextTimeout = std::min(timeout == nullptr ? TP::duration::max() : duration_cast<TP::duration>(*timeout), dura_of_timer);
    auto nextTimeoutMilliSeconds = duration_cast<milliseconds>(nextTimeout);
    //LOG(INFO) << "nextTimeout count: " << nextTimeout.count();
    //LOG(INFO) << "nextTimeoutMilliSeconds count: " << nextTimeoutMilliSeconds.count();
    if (nextTimeout.count() < 0 || nextTimeoutMilliSeconds.count() <= 0) {
      //LOG(INFO) << "epoll will directly dispatch timeout event, cause nextTimeout has expired...";
    } else {
      //LOG(INFO) << "epoll will wait: " << nextTimeoutMilliSeconds.count() << " milli seconds";
    }
    if (nextTimeout.count() > 0 && nextTimeoutMilliSeconds.count() > 0) {
      n = this->epoll_wait(nextTimeoutMilliSeconds.count());
    } else {
      //LOG(INFO) << "dispatching only timeout events";
      onlyTimeoutEvent = true;
    }
  }

  if (onlyTimeoutEvent || n == 0) {
    return dispatch_timeout_events();
  } else if(n < 0) {
    if(n == -2) {
      LOG(INFO) << "no events to wait, exiting...";
      return -1;
    }
    LOG(WARNING) << "Epoll_wait returned 0 or -1" << strerror(errno);
    return -1;
  } else if(n > 0) {
    n = this->dispatch(n);
  }
  return 0;
}

int epoll_reactor_impl::register_handler(EventHandler* handler, Event_Type type)
{
  std::lock_guard<std::mutex> guard(mutex_);
  if (handler == nullptr || type != EventHandler::TIMEOUT_EVENT)
  {
    LOG(WARNING) << "can't register none timeout handler with no handle";
    return -1;
  }
  demux_table_.bindNew(INVALID_HANDLE, type, handler);
  return 0;
}

int epoll_reactor_impl::unregister_handler(EventHandler *handler, Event_Type type)
{
  std::lock_guard<std::mutex> guard(mutex_);
  if (handler == nullptr || type != EventHandler::TIMEOUT_EVENT)
  {
    LOG(WARNING) << "can't unregister none timeout handler with no handle";
    return -1;
  }
  demux_table_.unbind(INVALID_HANDLE, type, handler);
  return 0;
}

int epoll_reactor_impl::register_handler(int handle, EventHandler *handler, Event_Type type)
{
  std::lock_guard<std::mutex> guard(mutex_);

  if(handle == INVALID_HANDLE || handler == 0 || type == EventHandler::NONE){
    LOG(ERROR) << "Handle error or registered type error...";
    return -1;
  }
  //LOG(INFO) << "Registering handler, handle: " << handle << " event: " << event_type_to_string(type);

  //already existed in the table
  if(demux_table_.getHandler(handle, type) != 0)
  {
    LOG(WARNING) << "Already existed in the demultiplex table, handle: "
      << handle << " event: " << event_type_to_string(type);
    return -1;
  }

  int ret = epoller_.add(handle, reactor_event_to_poll_event(type, USING_EPOLL));

  // memset(&cur_event_, 0, sizeof(struct epoll_event));

  // if(type & EventHandler::READ_EVENT)
  // {

  // }
  // else if(type & EventHandler::WRITE_EVENT)

  // cur_event_.events = reactor_event_to_poll_event(type, USING_EPOLL);
  // void *p_fd = &cur_event_.data.u64;
  // *(int*)p_fd = handle;
  // *(unsigned int*)(static_cast<char*>(p_fd) + 4) = type;
  // // cur_event_.data.u64 = (uint64_t)handle << 32 + type;


  // int ret = 0;
  // if(!demux_table_.has_handle(handle))
  //     ret = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &cur_event_);
  // else
  //     ret = ::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &cur_event_);

  if(ret < 0)
  {
    LOG(ERROR) << "Epoll_ctl add handle error... " << strerror(errno);
    return -1;
  }

  ret = this->demux_table_.bindNew(handle, type, handler);
  if(ret == 0)
    fd_count_++;
  return ret;
}

int epoll_reactor_impl::unregister_handler(int handle, EventHandler *handler, Event_Type type)
{
  std::lock_guard<std::mutex> guard(mutex_);

  if(handle == INVALID_HANDLE || handler == 0 || type == EventHandler::NONE){
    LOG(ERROR) << "Handle error or unregistered type error...";
    return -1;
  }
  //LOG(INFO) << "Unregistering handler, handle: " << handle << " event: " << event_type_to_string(type);

  //didn't find the handle and handler
  if(demux_table_.getHandler(handle, type) == 0)
  {
    LOG(WARNING) << "Can't unregister, didn't find the handle: "
      << handle << " event: " << event_type_to_string(type);
    return -1;
  }

  int ret = epoller_.del(handle, reactor_event_to_poll_event(type, USING_EPOLL));

  if(ret < 0)
  {
    LOG(ERROR) << "Epoll_ctl del handle error... " << strerror(errno);
    return -1;
  }

  ret = this->demux_table_.unbind(handle, type, handler);
  if(ret == 0)
    fd_count_--;
  return ret;
}

int epoll_reactor_impl::epoll_wait(int milliseconds)
{
  // LOG(INFO) << "Epoll waiting...";
  {
    // std::lock_guard<std::mutex> guard(mutex_);
    ret_events_.clear();
    ret_events_.resize(fd_count_);
  }
  if(fd_count_ == 1) return -2;

  // int ret = ::epoll_wait(epoll_fd_, &ret_events_[0], fd_count_, milliseconds == 0 ? -1 : milliseconds);
  int ret = epoller_.epoll_wait(&ret_events_[0], fd_count_, milliseconds == 0 ? -1 : milliseconds);

  if(ret < 0)
  {
    LOG(WARNING) << "Epoll error " << strerror(errno);
    if(errno == ERESTART || errno == EINTR) return 0;
    return -1;
  }

  if(ret == 0 && milliseconds != 0)
  {
    //LOG(WARNING) << "Poll timed out...";
    return 0;
  }

  return ret;
}

int epoll_reactor_impl::dispatch(int active_handle_count)
{
  int handles_dispatched = 0;
  int ret = this->dispatch_io_handlers(active_handle_count, handles_dispatched);
  if(ret != 0)
  {
    LOG(WARNING) << "Dispatch io handlers error";
    return -1;
  }
  return 0;
}

int epoll_reactor_impl::dispatch_io_handlers(int active_handles, int& handles_dispatched)
{
  (void)handles_dispatched;
  int remain = 0;
  remain = this->dispatch_io_epoll_sets(active_handles, 0, EPOLLOUT, &EventHandler::handle_output);

  // if (remain > 0)
  remain = this->dispatch_io_epoll_sets(remain, 0, EPOLLIN, &EventHandler::handle_input);

  if(remain > 0)
    remain = this->dispatch_io_epoll_sets(remain, 0, EPOLLERR, &EventHandler::handle_output);

  return remain;
}

int epoll_reactor_impl::dispatch_timeout_events()
{
  auto latestTimeout = demux_table_.getLastestTimeoutPoint();
  int ret = 1;
  while(latestTimeout < cached_now_)
  {
    auto handleSet = demux_table_.getLatestTimeoutHandlers();
    for(auto* h : handleSet)
    {
      ret = h->handle_timeout(-1) || ret;
    }
    demux_table_.unbindTimeoutHandlers(latestTimeout);
    latestTimeout = demux_table_.getLastestTimeoutPoint();
  }
  return ret;
}

int epoll_reactor_impl::dispatch_io_epoll_sets(int active_handles, int handles_dispatched, Event_Type type, HANDLER callback)
{
  epoll_event* ep_event_dispatching = nullptr;
  int current_fd = -1;
  int ret = -1;


  for(size_t i = 0; i < ret_events_.size(); i++)
  {
    if(active_handles - handles_dispatched <= 0)
      break;

    // mutex_.lock();

    ep_event_dispatching = &ret_events_[i];
    current_fd = ep_event_dispatching->data.fd;
    if(!(ep_event_dispatching->events & type))
    {
      // mutex_.unlock();
      continue;
    }

    handles_dispatched++;

    // LOG(INFO) << "Dispatching handle: " << current_fd << " event: " << event_type_to_string(type);

    EventHandler* handler = demux_table_.getHandler(current_fd, type);
    if(handler != nullptr)
      ret = (handler->*callback)(current_fd);

    // mutex_.unlock();

    if(ret < 0 && (handler != nullptr)) {
      // LOG(INFO) << "Unbinding handle: " << current_fd << " event: " << event_type_to_string(type);

      if(type == EventHandler::READ_EVENT)
        handler->close_read(current_fd);
      else if(type == EventHandler::WRITE_EVENT)
        handler->close_write(current_fd);
    }
    else {
      //LOG(INFO) <<"Keep listening on handle: " << current_fd << " event: " << event_type_to_string(type);
    }
    bool isHasHandle = demux_table_.hasHandle(current_fd);
    if(!isHasHandle && (handler != nullptr)) {
      LOG(INFO) << "closing handle: " << current_fd;
      handler->handle_close(current_fd);
    }
  }
  return active_handles - handles_dispatched;
}

}//namespace reactor
