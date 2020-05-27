#include "reactor/connection_handler.h"
#include "net/stream.h"
#include "reactor/reactor.h"

namespace reactor
{
const unsigned int connection_handler::BUFFER_HIGH_WATER_MARK = 100 * buffer_chain::MAXIMUM_CHAIN_SIZE;

uint32_t connection_handler::read(char *data_out, uint32_t data_len)
{
  if (data_out == 0 || data_len == 0)
    return -1;

  std::lock_guard<std::mutex> gurad{input_mutex_};
  if (input_buffer_.buffer_length() == 0)
    return 0;

  uint32_t buf_len = input_buffer_.buffer_length();
  uint32_t len_gonna_pullup = data_len;
  if (buf_len < data_len)
    len_gonna_pullup = buf_len;

  input_buffer_.remove(data_out, len_gonna_pullup);
  return len_gonna_pullup;
}

uint32_t connection_handler::read_line(char *data_out, uint32_t data_len, buffer_eol_style eol)
{
  if (data_out == 0 || data_len == 0)
    return -1;

  std::lock_guard<std::mutex> gurad{input_mutex_};
  return input_buffer_.read_line(data_out, data_len, eol);
}

uint32_t connection_handler::write(const char *data, uint32_t len, bool is_flush)
{
  std::lock_guard<std::mutex> gurad{output_mutex_};
  if (data == 0 || len == 0)
    return -1;

  if (output_buffer_.buffer_length() >= BUFFER_HIGH_WATER_MARK) {
    LOG(WARNING) << "Output buffer length got to HIGH_WATER_MARK";
    return 0;
  }

  //LOG(INFO) << "Writing data " << output_buffer_.buffer_length();
  auto bytesWritten = output_buffer_.append(data, len);
  if (is_flush && !write_enabled_)
    enable_writing();
  return bytesWritten;
}

int connection_handler::open()
{
  return enable_reading();
}

void connection_handler::close()
{
  if (read_enabled_)
    disable_reading();
  if (write_enabled_)
    disable_writing();
  close_stream();
}

void connection_handler::close_stream()
{
  //stream_->close();
}

int connection_handler::close_read(int)
{
  if(!stream_) 
    return -1;
  if (read_enabled_) disable_reading();
  stream_->close_read();
  return 0;
}
int connection_handler::close_write(int)
{
  if(!stream_) return -1;
  if (write_enabled_) disable_writing();
  stream_->close_write();
  return 0;
}

int connection_handler::enable_reading()
{
  if (read_enabled_ == true)
    return 0;
  read_enabled_ = true;
  return reactor_->register_handler(stream_->get_handle(), this, EventHandler::READ_EVENT);
}
int connection_handler::enable_writing()
{
  if (write_enabled_ == true) 
    return 0;
  write_enabled_ = true;
  return reactor_->register_handler(stream_->get_handle(), this, EventHandler::WRITE_EVENT);
}

int connection_handler::disable_reading()
{
  if (read_enabled_ == false) 
    return 0;
  read_enabled_ = false;
  int ret = reactor_->unregister_handler(stream_->get_handle(), this, EventHandler::READ_EVENT);
  //    check_and_invoke_close_callback();
  return ret;
}

int connection_handler::disable_writing()
{
  if (write_enabled_ == false)
    return 0;
  write_enabled_ = false;
  int ret = reactor_->unregister_handler(stream_->get_handle(), this, EventHandler::WRITE_EVENT);
  //    check_and_invoke_close_callback();
  return ret;
}
}
