#include "reactor/file_connection_handler.h"
#include "net/file_stream.h"
#include "net/unp.h"

namespace reactor
{

FileConnectionHandler::FileConnectionHandler(Reactor &react)
  : connection_handler(react)
{
  init_stream();
}

FileConnectionHandler::~FileConnectionHandler()
{}

int FileConnectionHandler::handle_input(int handle)
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
  recv_buf_size = recv_buf_size == 0 ? 4096 : recv_buf_size;

  stream_->setNonBolcking();
  int ret = stream_->read(input_buffer_, recv_buf_size);
  stream_->restoreBlocking();
  if (ret < 0) {
    if(errno == EAGAIN) {
      LOG(INFO) << "Read got EAGAIN...";
      errno = 0;
      return 0;
    }
    LOG(ERROR) << "Read error: " << strerror(errno) << "handle: " << handle;
    return -1;
  }
  if (ret == 0) {
    LOG(INFO) << "Read EOF";
    return -1;
  }
  return 0;
}

int FileConnectionHandler::handle_output(int)
{
  THROW_NOT_IMPLETED_EXCEPTION;
}

int FileConnectionHandler::handle_timeout(int) noexcept
{
  return 0;
}
int FileConnectionHandler::handle_close(int)
{
  close();
  return 0;
}
int FileConnectionHandler::handle_signal(int)
{
  THROW_NOT_IMPLETED_EXCEPTION;
}
int FileConnectionHandler::get_handle() const
{
  return stream_->get_handle();
}
void FileConnectionHandler::set_handle(int)
{
  THROW_NOT_IMPLETED_EXCEPTION;
}

void FileConnectionHandler::init_stream()
{
  stream_ = new FileStream();
}

int FileConnectionHandler::open_file(const char* file_path, int flags)
{
  auto *s = dynamic_cast<FileStream*>(stream_);
  s->set_file_stream_info(file_path, flags);
  return stream_->open();
}
}
