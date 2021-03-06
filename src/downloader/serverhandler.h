#pragma once
#include "proto/mess.pb.h"
#include "proto/mess_wl.pb.h"
#include "reactor/sock_connection_handler.h"
#include "download.h"
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace downloader
{

class DownloaderServerHandler :
    public reactor::sock_connection_handler,
    public downloader::Download::DownloadStateCallback,
    public std::enable_shared_from_this<DownloaderServerHandler>
{
public:

    using Downloader_t = downloader::Download;
    using Downloader_ptr_t = std::weak_ptr<Downloader_t>;
    DownloaderServerHandler(reactor::Reactor &react);

    ~DownloaderServerHandler(){}

    virtual int handle_input(int handle) override;
    virtual int handle_output(int handle) override;
    virtual int handle_close(int handle) override;

    virtual int open() override;

    virtual void taskAdded(int) override{}
    virtual void taskUpdated(int id, float finishPercent) override;
    virtual void taskFailed(int, const std::string&) override;
    virtual void taskPaused(int) override{}
    virtual void taskRemove(int)override{}
    virtual void taskCompleted(int id)override;
    virtual void taskResumed(int) override{}

private:
    void destroy();
    int decode();
    void saveCurrentMess();
	void dispatchMessage(downloadmessage::Mess_WL& mes);
    void sendResponseMess(int id, float percent, downloadmessage::Download_Response_State state);
    bool isWritable() const;

private:
	int bytesParsed_ = 0;
    downloadmessage::Mess_WL currentMess_;
    std::queue<downloadmessage::Mess_WL*> downloadQueue_;
    std::vector<Downloader_ptr_t> dPtrs_;
    std::unordered_map<int, Downloader_ptr_t> downloaderMap_;
    bool completed_ = false;
    std::mutex mutex_;
};

} // namespace downloader
