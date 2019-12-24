#pragma once
#include "reactor/connection_handler.h"

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
};
}
