#include "serverhandler.h"
#include "proto/mess.pb.h"

namespace downloader
{

DownloaderServerHandler::DownloaderServerHandler(reactor::Reactor& react) 
    : connection_handler(react)
{
}

int DownloaderServerHandler::open()
{
    return enable_reading();
}

int DownloaderServerHandler::handle_input(int handle)
{
    auto ret = connection_handler::handle_input(handle);
    if(ret < 0) return -1;

    if(input_buffer_.buffer_length() == 0) return 0;

    while(input_buffer_.buffer_length() > 0)
    {
        auto firstChain = input_buffer_.begin().chain();
        auto data = firstChain.get_start_buffer();
        auto chainLen = firstChain.size();
        long sizeInMes = 0;
        httpmessage::Mess *mes = new httpmessage::Mess{};
        //if the size we received is not enough for a mess
        if(chainLen < (mes->ByteSizeLong() - sizeInMes))
        {
            mes->ParsePartialFromArray(data, chainLen);
            input_buffer_.drain(chainLen);
        }else
        {
            mes->ParseFromArray(data, mes->ByteSizeLong());
            input_buffer_.drain(mes->ByteSizeLong());
        }
    }
    return 0;
}


}