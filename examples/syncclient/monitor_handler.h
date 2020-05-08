#pragma once
#include "proto/decoder.h"
#include "reactor/connection_handler.h"
#include "reactor/reactor.h"
#include "proto/sync_package.h"
#include "examples/dirmonitor/DirMonitor.h"

namespace filesync
{

class FileMonitorHandler : public reactor::connection_handler, public IDirObserver
{
public:
	virtual void onUpdate(const EntryMap& es);
  FileMonitorHandler(reactor::Reactor& react);
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
