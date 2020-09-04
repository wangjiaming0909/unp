#pragma once
#include "proto/decoder.h"
#include "reactor/ConnectionManager.h"
#include "reactor/sock_connection_handler.h"
#include "proto/sync_package.h"
#include "util/FileWriter.h"

#include <map>
#include <list>
#include <boost/filesystem.hpp>
#include <chrono>

namespace filesync
{

enum DirEntryStatus
{
  idle, syncing, syncInterrupted, syncFailed, syncSucceed
};

class SyncServerHandler : public reactor::sock_connection_handler
{
public:
  using DirE_t = boost::filesystem::directory_entry;
  SyncServerHandler(reactor::Reactor& react);

public:
  virtual int handle_input(int handle) override;

private:
  void sayHello();
  uint64_t sendPackage(SyncPackagePtr package);
  bool check_send_response();
  int write_file(const char* file_name, const char *data, size_t size);
  void handle_deposite_file(SyncPackagePtr mess);

private:
  DirE_t localStoreDirectory_;
  //std::list<DirE_t*> receivedEntries_;
  reactor::Decoder<SyncPackage, int64_t> decoder_;
  //reactor::Reactor *file_reactor_ = nullptr;
  //std::unique_ptr<reactor::ConnectionManager> manager_;
  std::chrono::system_clock::time_point cached_time_;
  static const int check_response_time_interval_;
  std::map<std::string, utils::FileWriter*> file_writers_;
};
}
