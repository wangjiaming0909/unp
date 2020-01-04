#pragma once
#include "proto/mess.pb.h"
#include "proto/mess_wl.pb.h"
#include "reactor/connection_handler.h"
#include "download.h"
#include <queue>
#include <memory>
#include <thread>

namespace downloader
{

class DownloaderServerHandler : public reactor::connection_handler, public downloader::Download::DownloadStateCallback
{
public:
    struct DownloadStateCallback
    {
        virtual void downloadStarted(int32_t id) = 0;
        //virtual void 
    };
public:

    using Downloader_t = downloader::Download;
    using Downloader_ptr_t = std::shared_ptr<Downloader_t>;
    DownloaderServerHandler(reactor::Reactor &react);

    ~DownloaderServerHandler(){}

    virtual int handle_input(int handle) override;
    virtual int handle_output(int handle) override;
    virtual int handle_close(int handle) override;

    virtual int open() override;

    virtual void taskAdded(int id) override{}
    virtual void taskFailed(int id) override{}
    virtual void taskPaused(int id) override{}
    virtual void taskRemove(int id)override{}
    virtual void taskCompleted(int id)override;
    virtual void taskResumed(int) override{}

private:
    void destroy();
    int decode();
    void saveCurrentMess();
	void dispatchMessage(downloadmessage::Mess_WL& mes);

private:
	int bytesParsed_ = 0; 
    downloadmessage::Mess_WL currentMess_;
    std::queue<downloadmessage::Mess_WL*> downloadQueue_;
    Downloader_ptr_t dPtr_;
    std::shared_ptr<std::thread> threadPtr_;
    bool completed_ = false;
};

} // namespace downloader
