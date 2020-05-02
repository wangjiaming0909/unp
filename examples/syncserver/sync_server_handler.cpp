#include "syncserver/sync_server_handler.h"
#include "proto/decoder.h"

namespace filesync
{

SyncServerHandler::SyncServerHandler(reactor::Reactor& react)
    : connection_handler(react)
    , localStoreDirectory_{boost::filesystem::current_path()}
    , receivedEntries_{}
{
}

int SyncServerHandler::handle_input(int handle)
{
    int ret = connection_handler::handle_input(handle);
    if (ret < 0)
    {
        return ret;
    }

    if (input_buffer_.buffer_length() > 0)
    {
        //input_buffer_.readline()
        auto firstChain = input_buffer_.begin().chain();
        auto data = firstChain.get_start_buffer();
        auto chainLen = firstChain.size();

        auto lenParsed = decoder_.decode(static_cast<char*>(data), chainLen);
        input_buffer_.drain(lenParsed);
        if (decoder_.isCompleted()) 
        {
          LOG(INFO) << decoder_.getMess().content();
        }
    }

    return 0;
}




}
