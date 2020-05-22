#include "serverhandler.h"
#include "util/easylogging++.h"
#include <cassert>
#include <memory>
#include <mutex>
#include "d.h"

namespace downloader
{
using namespace downloadmessage;

DownloaderServerHandler::DownloaderServerHandler(reactor::Reactor& react) 
    : sock_connection_handler(react)
    , currentMess_{}
    , downloaderMap_{}
    , mutex_{}
{
}

int DownloaderServerHandler::open()
{
    LOG(INFO) << "DownloaderServerHandler::open()";
    return enable_reading();
}

int DownloaderServerHandler::handle_input(int handle)
{
    LOG(INFO) << "DownloaderServerHandler::handler_input()";
    auto ret = sock_connection_handler::handle_input(handle);
    if(ret < 0) return -1;

	if (input_buffer_.buffer_length() == 0) return 0;

    while(input_buffer_.buffer_length() > 0 && decode() >= 0)
    {
        dispatchMessage(currentMess_);
        currentMess_.Clear();
    }
    return 0;
}

int DownloaderServerHandler::handle_output(int handle)
{
    std::lock_guard<std::mutex> guard{mutex_};
    //LOG(INFO) << "DownloaderServerHandler::handle_output()";
    if(sock_connection_handler::handle_output(handle) != 0)
    {
        LOG(ERROR) << "output error...";
        return -1;
    }
    if(completed_)
    {
        destroy();
    }
    return 0;
}

int DownloaderServerHandler::decode()
{
	uint32_t bytesRemainToParse = 0;
	if(bytesParsed_ == 0)//from start of a message
	{
		auto data = input_buffer_.pullup(sizeof(int32_t));
		currentMess_.ParsePartialFromArray(data, sizeof(int32_t));
		bytesParsed_ = sizeof(int32_t);
		input_buffer_.drain(sizeof(int32_t));
		if (!currentMess_.has_len())
		{
			LOG(ERROR) << "parse message len error";
			return -1;
		}
		bytesRemainToParse = currentMess_.len() - sizeof(int32_t);
	} else {// from some pos of a message
		bytesRemainToParse = currentMess_.len() - bytesParsed_;
	}
	
	assert(input_buffer_.buffer_length() > 0);
	int32_t bytesGoingToParse = std::min(bytesRemainToParse, input_buffer_.buffer_length());
	auto d = input_buffer_.pullup(bytesGoingToParse);
	Mess_WL tm{};
	tm.ParsePartialFromArray(d, bytesGoingToParse);
	input_buffer_.drain(bytesGoingToParse);
	bytesParsed_ += bytesGoingToParse;
	currentMess_.MergeFrom(tm);
	if(bytesParsed_ != currentMess_.len())// only when the mes has been parsed thoroughly
	{
        LOG(INFO) << "mess not completed...";
        return -1;
	}
    LOG(INFO) << "one mess parsed...";
    bytesParsed_ = 0;//one message has parsed already, set bytesParsed_ to 0
    return 0;
}

int DownloaderServerHandler::handle_close(int fd)
{
    LOG(INFO) << "DownloaderServerhandler::Handle_close()";
    return sock_connection_handler::handle_close(fd);
}

void DownloaderServerHandler::dispatchMessage(Mess_WL& mes)
{
    auto command = mes.command();
    int id = mes.id();
    if(downloaderMap_.count(id) != 0)
    {
        LOG(INFO) << "id exited...";
        return;
    }
    LOG(INFO) << "dispatching message id: " << id;
    switch(command)
    {
        case(Mess_WL::DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD):
            {
                LOG(INFO) << "downloading: " << mes.url();
                auto downloaderPtr = std::make_shared<Downloader_t>(id, mes.url(), this->shared_from_this());
                downloaderMap_[id] = Downloader_ptr_t(downloaderPtr);
                LOG(INFO) << "adding one download task";
                Pool::pool->add_task(
                    [=]() {
                        downloaderPtr->download();
                    });
            }
            break;
        case (Mess_WL::DownloadCommand::Mess_WL_DownloadCommand_PAUSE):
            LOG(INFO) << "pause: " << mes.url();
            break;
        case (Mess_WL::DownloadCommand::Mess_WL_DownloadCommand_RESUME):
            LOG(INFO) << "resume: " << mes.url();
            break;
        case (Mess_WL::DownloadCommand::Mess_WL_DownloadCommand_REMOVE):
            LOG(INFO) << "remove: " << mes.url();
            break;
        default:
            LOG(ERROR) << "unknown command...";
            return;
    }
}

void DownloaderServerHandler::saveCurrentMess() { }

void DownloaderServerHandler::destroy()
{
    sock_connection_handler::handle_close(stream_->getHandle());
}

//this will be called in another thread
void DownloaderServerHandler::taskCompleted(int id)
{
    std::lock_guard<std::mutex> guard{mutex_};
    if(!this->stream_->hasHandle() || !stream_->get_handle().canWrite())
    {
        //LOG(WARNING) << "Sending task completed, but peer socket has been closed...";
        return;
    }
    sendResponseMess(id, 1.0, Download_Response_State_FINISHED);
    if(downloaderMap_.size() == 0)
    {
        destroy();
    }
}

bool DownloaderServerHandler::isWritable() const
{
    if(!this->stream_->hasHandle() || !stream_->get_handle().canWrite())
    {
        return false;
    }
    return true;
}

void DownloaderServerHandler::taskUpdated(int id, float finishPercent)
{
    static uint64_t counter = 0;
    std::lock_guard<std::mutex> guard{mutex_};
    counter++;
    if(!isWritable()) return;
    // LOG(INFO) << "finish percent: " << finishPercent;
    if(counter % 1000 != 0 && (1.0 - finishPercent) > 0.0001)
    {
        return;
    }

    // LOG(INFO) << "task: " << id << " updating finished: " << finishPercent;
    sendResponseMess(id, finishPercent, Download_Response_State_DOWNLOADING);
}

void DownloaderServerHandler::sendResponseMess(int id, float percent, Download_Response_State state)
{
    if(!isWritable()) return;
    Download_Response resp{};
    resp.set_state(state);
    resp.set_id(id);
    resp.set_percent(percent);
    char arr[128] = {};
    resp.SerializeToArray(arr, 128);
    auto ret = write(arr, sizeof(int) + sizeof(float) + sizeof(Download_Response_State));
    if(ret == 0)
    {
        LOG(ERROR) << "Write update mes error";
    }
}

void DownloaderServerHandler::taskFailed(int id, const std::string& mes)
{
    if(!isWritable()) return;
    sendResponseMess(id, 0, Download_Response_State_FAILED);
}

}
