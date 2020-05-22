#pragma once
#include <memory>
#include <mutex>

#include "reactor/EventHandler.h"
#include "reactor/buffer.h"

namespace reactor
{
class Stream;
class connection_handler : public EventHandler
{
public:
  connection_handler(Reactor& react) : EventHandler(react){}
  virtual ~connection_handler() = default;

  Stream* get_stream() { return stream_; }

  virtual uint32_t read(char *data_out, uint32_t data_len);
  virtual uint32_t read_line(char *data_out, uint32_t data_len, buffer_eol_style eol);
  virtual uint32_t write(const char *data, uint32_t len, bool is_flush = true);
  template <typename T>
  int write(const T &data, bool is_flush = false);

  virtual int open();
  virtual void close();
  virtual void init_stream() = 0;
  virtual void close_stream();
  virtual int close_read(int) override;
  virtual int close_write(int) override;

  int enable_reading();
  int enable_writing();
  int disable_reading();
  int disable_writing();

  void clear_input_buffer()
  {
    input_buffer_.drain(1);
  }

protected:
  Stream* stream_;
  buffer input_buffer_;
  buffer output_buffer_;
  std::mutex output_mutex_;
  std::mutex input_mutex_;
  bool read_enabled_ = false;
  bool write_enabled_ = false;
  static const unsigned int BUFFER_HIGH_WATER_MARK;
};

template <typename T>
int connection_handler::write(const T &data, bool is_flush)
{
  std::lock_guard<std::mutex> gurad(output_mutex_);
  output_buffer_.append(data);
  if (is_flush && !write_enabled_)
    enable_writing();
  return sizeof(data);
}

}
