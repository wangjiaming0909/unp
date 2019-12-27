#pragma once
#include "proto/mess.pb.h"
#include "reactor/connection_handler.h"
#include <queue>

namespace downloader
{

class DownloaderServerHandler : public reactor::connection_handler
{

public:
    DownloaderServerHandler(reactor::Reactor &react);

    ~DownloaderServerHandler(){}

    virtual int handle_input(int handle) override;

    virtual int open() override;

private:
    void saveCurrentMess();

private:
    downloadmessage::Mess currentMess_;
    std::queue<downloadmessage::Mess*> downloadQueue_;
};
}
