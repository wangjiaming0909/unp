#pragma once
#include "proto/mess.pb.h"
#include "proto/mess_wl.pb.h"
#include "reactor/connection_handler.h"
#include "download.h"
#include <queue>
#include <memory>

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
	int decode();
    void saveCurrentMess();
	void dispatchMessage(downloadmessage::Mess_WL& mes);

private:
	int bytesParsed_ = 0; 
    downloadmessage::Mess_WL currentMess_;
    std::queue<downloadmessage::Mess_WL*> downloadQueue_;
	std::shared_ptr<downloader::Download> d_;
};
}
