#include "syncclient/sync_file_connection_handler.h"
#include "boost/filesystem/directory.hpp"
#include "proto/sync_package.h"
#include "syncclient/monitor_handler.h"

namespace filesync
{
SyncFileConnectionHandler::SyncFileConnectionHandler(
    Reactor& react
    , FileMonitorHandler& output_connection
    , const std::string& file_name
    , uint64_t file_size)
  : FileConnectionHandler(react)
  , output_connection_(&output_connection)
  , file_name_(file_name)
  , file_len_(file_size)
{

}

int SyncFileConnectionHandler::post_handle_input(int handle)
{
  NOT_USED(handle);
  auto buffer_len = input_buffer_.buffer_length();
  if (buffer_len == 0) {
    return 0;
  }

  auto* data = input_buffer_.pullup(buffer_len);
  auto from = bytes_sent_ + 1;
  auto to  = from + buffer_len;
  auto package = getDepositeFilePackage(file_name_.c_str(), file_len_, from, to, data);

  int64_t size = package->ByteSizeLong();
  char* d = static_cast<char*>(::calloc(size, 1));
  package->SerializeToArray(d, size);
  auto bytes_written = output_connection_->write(size, false);
  bytes_written += output_connection_->write(d, size, true);
  free(d);
  if (bytes_written <= 0) {
    LOG(WARNING) << "SyncFileConnectionHandler::post_handle_input write returned: " << bytes_written;
    return -1;
  }
  input_buffer_.drain(buffer_len);
  bytes_sent_ += bytes_written;
  return 0;
}

int SyncFileConnectionHandler::handle_close(int handle)
{
  close();
  boost::filesystem::path p(file_name_);
  return output_connection_->add_to_finished(boost::filesystem::directory_entry(p));
}
}
