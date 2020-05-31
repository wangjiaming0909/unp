#include "syncclient/sync_file_connection_handler.h"
#include "proto/sync_package.h"

namespace filesync
{
SyncFileConnectionHandler::SyncFileConnectionHandler(
    Reactor& react
    , connection_handler& output_connection
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
}
}
