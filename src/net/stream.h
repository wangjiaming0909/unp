#pragma once
#include "boost/noncopyable.hpp"
#include <boost/core/noncopyable.hpp>

namespace reactor
{

class buffer;
struct Stream : public boost::noncopyable
{
  virtual void open() = 0;
  virtual void close() = 0;
  virtual ssize_t read(void *buffer, size_t len) = 0;
  virtual ssize_t read(reactor::buffer &buf, size_t len) = 0;
  virtual ssize_t write(const void *buffer, size_t len) = 0;
  virtual ssize_t writev_n(const void *buffer, size_t len) = 0;
  virtual int getHandle() const = 0;
  virtual bool hasHandle() const = 0;
  virtual int setNonBolcking() = 0;
  virtual int restoreBlocking() = 0;
};
}
