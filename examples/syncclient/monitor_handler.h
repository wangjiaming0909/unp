#pragma once
#include "proto/decoder.h"
#include "reactor/sock_connection_handler.h"
#include "reactor/reactor.h"
#include "proto/sync_package.h"
#include "examples/dirmonitor/DirMonitor.h"
#include "net/unp.h"
#include "reactor/ConnectionManager.h"
#include "reactor/file_reader.h"
#include "sync_file_connection_handler.h"
#include <condition_variable>
#include <memory>
#include <mutex>

namespace filesync
{

// DirMonitor thread will add modified entries into FileMonitorHandler
// which thread will pack the `send file packages` and write it to output_buffer_? FileMonitor thread or reactor thread or a new thread?
//  using FileMonitor thread:
//    FileMonitor thread need to read all contents of files that need to sync, and write it to sock_connection_handler's out buffer
//    reading and writing takes lots of time, and output_buffer_ could be full,
//    when output_buffer_ is full, how to send the remaining contents?
//
//  using reactor thread:
//    how to use reactor thread to pack file packages?
//    how to trigger reactor thread to pack `send file packages`
//  using new thread:
//    FileMonitor thread update entries into FileMonitorHandler
//    new thread listen on the entries in FileMonitorHandler,
//    any onUpdate will trigger new thread to update the status of every Entry
//      if one Entry need sync, this thread will try to read the contents of the file, write it to FileMonitorHandler
//      if one Entry is deleted which is syncing or synced, delete it from syncing list or delete from server
//      this thread need to do some accounting job to record the synced offset of entries, it will be useful when output_buffer_ is full
//      as long as there are syncing tasks need to be done, this thread will continue to read from file and write to output_buffer_
//    where should we put this new thread?
class FileMonitorHandler : public reactor::sock_connection_handler, public IDirObserver
{
public:
  using ReaderType = reactor::FileReader<SyncFileConnectionHandler>;
  FileMonitorHandler(reactor::Reactor& react, IDirObservable& observable);
  virtual void onUpdate(const EntryMap& es) override;
  virtual int handle_close(int handle) override;

  int add_to_finished(const Entry& e);

private:
  void add_to_need_sync(const Entry& e);
  void add_to_pause(const Entry& e);
  void add_to_error(const Entry& e);
  void sync();
  void close_reader(const std::string& file_name);

private:
  EntryMap entries_;
  std::set<Entry> pending_sync_set_;
  std::set<Entry> syncing_set_;
  std::set<Entry> finished_set_;
  std::set<Entry> paused_set_;
  std::set<Entry> error_set_;
  std::mutex pending_set_mutex_;
  std::condition_variable pending_set_cv_;
  reactor::Reactor *reactor_;
  bool cancel_reactor_token_ = false;
  std::unique_ptr<reactor::ConnectionManager> manager_;
  std::vector<ReaderType*> readers_;
  std::map<std::string, ReaderType*> readers_map_;
};

class ServerMonitorHandler : public reactor::sock_connection_handler
{
public:
enum class ServerStatus{idle, connected, disconnected, reconnecting};
public:
  ServerMonitorHandler(reactor::Reactor& react);

public:
  virtual int handle_input(int handle) override;
  virtual int open() override;
  ServerStatus getServerStatus() const {return serverStatus_;}

private:
  ServerStatus serverStatus_;
  reactor::Decoder<SyncPackage, int64_t> decoder_;
};

}
