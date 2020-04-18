#pragma once
#include "reactor/connection_handler.h"

#include <map>
#include <list>
#include <boost/filesystem.hpp>

namespace filesync
{

enum DirEntryStatus
{
    idle, syncing, syncInterrupted, syncFailed, syncSucceed
};

class SyncServerHandler : public reactor::connection_handler
{
public:
    using DirE_t = boost::filesystem::directory_entry;
    SyncServerHandler(reactor::Reactor& react);

public:
    virtual int handle_input(int handle) override;

private:
    DirE_t localStoreDirectory_;
    std::list<DirE_t*> receivedEntries_;
};
}