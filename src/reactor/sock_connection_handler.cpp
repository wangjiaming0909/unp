#include "reactor/sock_connection_handler.h"
#include "net/ssl_sock_stream.h"
#include "net/inet_sock_stream.h"
#include <mutex>

using namespace reactor;

sock_connection_handler::sock_connection_handler(Reactor &reactor, bool isSSL)
  : connection_handler(reactor)
  , isSSL_(isSSL)
{
  init_stream();
}

sock_connection_handler::~sock_connection_handler()
{
  close();
  close_stream();
}

int sock_connection_handler::handle_input(int handle)
{
  if(!stream_) 
    return -1;
  if (input_buffer_.total_len() >= connection_handler::BUFFER_HIGH_WATER_MARK)
    return 0;

  if (handle != stream_->get_handle() || handle == INVALID_HANDLE) {
    LOG(ERROR) << "Register error: handle error: " << handle;
    return -1;
  }

  int recv_buf_size = 0;
  // socklen_t optlen = sizeof(recv_buf_size);
  /*
     int ret = getsockopt(handle, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, 0);
     if(ret != 0)
     {
     LOG(ERROR) << "Get Rcv_buf size error: " << strerror(errno);
     }
     */
  recv_buf_size = recv_buf_size == 0 ? 4096 : recv_buf_size;

  stream_->setNonBolcking();
  int ret = stream_->read(input_buffer_, recv_buf_size);
  stream_->restoreBlocking();
  if (ret < 0) {
    if(errno == EAGAIN) {
      // LOG(INFO) << "Read got EAGAIN...";
      errno = 0;
      return 0;
    }
    LOG(ERROR) << "Read error: " << strerror(errno) << "handle: " << handle;
    return -1;
  }
  if (ret == 0) {
    //LOG(INFO) << "Read EOF";
    return -1;
  }
  return 0;
}

int sock_connection_handler::handle_output(int handle)
{
  std::lock_guard<std::mutex> gurad{output_mutex_};
  if (output_buffer_.buffer_length() == 0)
    return 0;

  if (handle != stream_->get_handle() || handle == INVALID_HANDLE) {
    LOG(ERROR) << "Register error: handle error: " << handle;
    return -1;
  }

  int try_times = 3;

#ifndef DEFAULT_SEND_SIZE
#define DEFAULT_SEND_SIZE 4096
#endif
  int bytes_send = 01;
  for (; try_times > 0; try_times--) {
    //行为： 最多pullup 4096 bytes
    size_t pullupSize = DEFAULT_SEND_SIZE > output_buffer_.buffer_length() ? output_buffer_.buffer_length() : DEFAULT_SEND_SIZE;
    auto data_p = output_buffer_.pullup(pullupSize);
    //LOG(INFO) << "sending " << output_buffer_.buffer_length();
    bytes_send = stream_->send(static_cast<const void *>(data_p), pullupSize, 0);
    if (bytes_send <= 0) {
      LOG(ERROR) << "Send error: " << strerror(errno) << " handle: " << handle;
      if(errno == ECONNRESET) {
        disable_reading();
        disable_writing();
        return -1;
      }
      LOG(INFO) << "retrying... " << try_times + 1 << " time";
      continue;
    }

    if (bytes_send < 0) {
      disable_writing();
      stream_->close_write();
      return -1;
    }

    //socket send buffer could be full, try 3 times, if can't send also, give up
    output_buffer_.drain(bytes_send);
    //although, we give up here
    //if using edge trigger, handle_output will be invoked immediately
    if (output_buffer_.buffer_length() == 0) {
      break;
    }
  }

  //if buffer has no data, disabling the writing event
  if (output_buffer_.buffer_length() == 0)
    if (disable_writing() != 0)
      LOG(ERROR) << "Disable writing error: " << strerror(errno);
  return 0;
}

int sock_connection_handler::handle_timeout(int) noexcept { return 0; }

int sock_connection_handler::handle_close(int)
{
  int handle = stream_->get_handle();
  close();
  check_and_invoke_close_callback(handle);
  return 0;
}

int sock_connection_handler::handle_signal(int) { return 0; }

int sock_connection_handler::get_handle() const
{
  return stream_->get_handle();
}

void sock_connection_handler::set_handle(int)
{
}

void sock_connection_handler::init_stream()
{
  if(isSSL_)
    stream_ = new net::SSLSockStream();
  else
    stream_ = new net::InetSockStream();
}
void sock_connection_handler::check_and_invoke_close_callback(int handle)
{
  // if(!read_enabled_ && !write_enabled_)
  //   // {
  if (closed_callback_)
    closed_callback_(handle);
  // }
}
