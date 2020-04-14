#include "syncserver/sync_server_handler.h"

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
        auto p = input_buffer_.pullup(1024);
        LOG(INFO) << "received: " << p;
    }

    return 0;
}




}