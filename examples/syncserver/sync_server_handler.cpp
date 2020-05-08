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

    while (input_buffer_.buffer_length() > sizeof(int64_t))
    {
        auto lenParsed = decoder_.decode(input_buffer_);
        if (lenParsed <= 0) return -1;
        auto mes = decoder_.getMess();
        if (mes)
        {
          if (mes->header().command() == Command::ClientHello)
          {
            LOG(INFO) << "Received client hello...";
            sayHello();
            LOG(INFO) << "Send server hello...";
          }
        }
        decoder_.reset();
    }
    return 0;
}

void SyncServerHandler::sayHello()
{
  const char* hello = "hello";
  auto helloPackage = getHelloPackage(hello, PackageType::Server);
  sendPackage(helloPackage);
}
}
