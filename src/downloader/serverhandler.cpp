#include "serverhandler.h"
#include "util/easylogging++.h"
#include <cassert>
#include <memory>
#include "d.h"

namespace downloader
{

DownloaderServerHandler::DownloaderServerHandler(reactor::Reactor& react) 
    : connection_handler(react)
    , currentMess_{}
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
    auto ret = connection_handler::handle_input(handle);
    if(ret < 0) return -1;

	if (input_buffer_.buffer_length() == 0) return 0;

	if (decode() != 0) return -1;
    return 0;
}

int DownloaderServerHandler::handle_output(int handle)
{
    LOG(INFO) << "DownloaderServerHandler::handle_output()";
    if(connection_handler::handle_output(handle) != 0)
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
	downloadmessage::Mess_WL tm{};
	tm.ParsePartialFromArray(d, bytesGoingToParse);
	input_buffer_.drain(bytesGoingToParse);
	bytesParsed_ += bytesGoingToParse;
	currentMess_.MergeFrom(tm);
	if(bytesParsed_ == currentMess_.len())// only when the mes has been parsed thoroughly
	{
		dispatchMessage(currentMess_);
		bytesParsed_ = 0;//one message has parsed already, set bytesParsed_ to 0
		return 0;
	}
	else return 0;
}

int DownloaderServerHandler::handle_close(int fd)
{
    LOG(INFO) << "DownloaderServerhandler::Handle_close()";
    // connection_handler::close();
    // int ret = 0;
    // if (threadPtr_)
    // {
    //     if (threadPtr_->joinable())
    //     {
    //         threadPtr_->detach();
    //         ret = 0;
    //     }
    //     else ret = -1;
    // }
    // else ret = -1;
    return 0;
}

void DownloaderServerHandler::dispatchMessage(downloadmessage::Mess_WL& mes)
{
    auto command = mes.command();
    using namespace downloadmessage;
    switch(command)
    {
        case(Mess_WL::DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD):
            LOG(INFO) << "downloading: " << mes.url();
            dPtr_ = std::make_shared<Downloader_t>(mes.url(), this);
            LOG(INFO) << "adding one download task";
            Pool::pool->add_task(
                [=]() {
                    dPtr_->download();
                });
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

void DownloaderServerHandler::saveCurrentMess()
{


}

void DownloaderServerHandler::destroy()
{
    if(threadPtr_ && threadPtr_->joinable())
    {
        threadPtr_->detach();
    }
    connection_handler::handle_close(stream_->getHandle());
}

void DownloaderServerHandler::taskCompleted(int id)
{
    LOG(INFO) << "completed task id: " << id;
    const char* mes = "F\n";
    // auto ret = write(mes, strlen(mes));
    completed_ = true;
    // if(ret == 0)//写入失败
    // {
        destroy();
    // }
}
}
