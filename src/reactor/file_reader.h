#pragma once
#include "reactor/service.h"

namespace reactor
{
template <typename Handler_T>
class FileReader : public ServiceT
{
public:
  FileReader(Handler_T* handler)
    : ServiceT()
    , handler_(handler)
  {}
  ~FileReader() = default;
public:
  virtual int open() override;
  virtual int close() override;

  Handler_T* get_connection_handler() {return handler_;}

protected:
  Handler_T* handler_;
};
}
