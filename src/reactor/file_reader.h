#pragma once
#include "reactor/service.h"
#include "net/unp.h"
#include <stdexcept>

namespace reactor
{
template <typename Handler_T>
class FileReader : public ServiceT
{
public:
  using HandlerT = Handler_T  ;
  FileReader(HandlerT& handler)
    : ServiceT()
    , handler_(&handler)
  {}
  ~FileReader() = default;
public:
  virtual int close() override
  {
    if (handler_)
    {
      handler_->close();
      return 0;
    }
    return -1;
  }

  int open_file(const char* file_name, int flags)
  {
    if (!handler_)
      return -1;
    handler_->open_file(file_name, flags);
    handler_->open();
  }

  Handler_T* get_connection_handler() {return handler_;}

private:
  virtual int open() override
  {
    if (handler_)
      return handler_->open();
  }

protected:
  Handler_T* handler_;
};
}
