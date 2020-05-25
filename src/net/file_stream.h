#pragma once
#include "net/stream.h"

namespace reactor
{
class FileStream : public Stream
{
public:
  FileStream() : Stream() {}
  ~FileStream() = default;
public:
  virtual int open() override;
  virtual int close() override;
  virtual ssize_t read(void *buffer, size_t len) override;
  virtual ssize_t read(reactor::buffer &buf, size_t len) override;
  virtual ssize_t write(const void *buffer, size_t len) override;
  virtual ssize_t writev_n(const void *buffer, size_t len) override;
  virtual int get_handle() const override;
  virtual bool has_handle() const override;
  virtual int setNonBolcking() override;
  virtual int restoreBlocking() override;
private:
  int handle_;
};
}
