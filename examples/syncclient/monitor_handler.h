#pragma once
#include "proto/decoder.h"
#include "reactor/connection_handler.h"
#include "reactor/reactor.h"
#include "proto/sync_package.h"
#include "examples/dirmonitor/DirMonitor.h"

namespace filesync
{

// DirMonitor thread will add modified entries into FileMonitorHandler
// which thread will pack the `send file packages` and write it to output_buffer_? FileMonitor thread or reactor thread or a new thread?
//  using FileMonitor thread:
//    FileMonitor thread need to read all contents of files that need to sync, and write it to connection_handler's out buffer
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
class FileMonitorHandler : public reactor::connection_handler, public IDirObserver
{
public:
  FileMonitorHandler(reactor::Reactor& react, IDirObservable& observable);
	virtual void onUpdate(const EntryMap& es) override;
  virtual int handle_close(int handle) override;

private:
  EntryMap entries_;
};

class ServerMonitorHandler : public reactor::connection_handler
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
