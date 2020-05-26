#pragma once
#include "net/stream.h"

namespace reactor
{
class FileStream : public Stream
{
public:
  FileStream(){}
  FileStream(const char* path, int flags);
  ~FileStream();
public:
  virtual int open() override;
  virtual int close() override;
  virtual ssize_t read(void *buffer, size_t len) override;
  virtual ssize_t read(reactor::buffer &buf, size_t len) override;
  virtual ssize_t write(const void *buffer, size_t len) override;

  virtual ssize_t writev_n(const void *buffer, size_t len) override;
  virtual ssize_t send(const void *buffer, size_t len, int flags) override;
  virtual ssize_t send_n(const void *buffer, size_t len, int flags) override;

  virtual int get_handle() const override;
  virtual bool has_handle() const override;
  virtual int set_handle(int handle) override;
  virtual void close_read() override;
  virtual void close_write() override;
  virtual int setNonBolcking() override;
  virtual int restoreBlocking() override;

  void set_open_flags(int flags);
private:
  int handle_;
};
}
