#pragma once
#include "reactor/file_connection_handler.h"

namespace filesync
{
class FileMonitorHandler;
using namespace reactor;
class SyncFileConnectionHandler : public FileConnectionHandler
{
public:
  SyncFileConnectionHandler(Reactor& react, FileMonitorHandler& output_connection, const std::string& file_name, uint64_t file_len);
  ~SyncFileConnectionHandler() = default;

  virtual int handle_close(int handle) override;

private:
  virtual int post_handle_input(int handle) override;
  uint64_t send_to_server();

private:
  FileMonitorHandler* output_connection_;
  std::string file_name_;
  uint64_t file_len_;
  uint64_t bytes_sent_ = -1;
};
}
