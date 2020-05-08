#include "../examples/syncserver/sync_server_handler.h"
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
        auto firstChain = input_buffer_.begin().chain();
        auto data = firstChain.get_start_buffer();
        auto chainLen = firstChain.size();

        auto lenParsed = decoder_.decode(input_buffer_);
        if (decoder_.getMess())
        {
          LOG(INFO) << decoder_.getMess()->header().command();
        }
    }
    return 0;
}




}
