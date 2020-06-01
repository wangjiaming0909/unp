#include "../examples/syncserver/sync_server_handler.h"
#include "proto/decoder.h"
#include "reactor/ConnectionManager.h"
#include "reactor/file_reactor_impl.h"
#include "reactor/reactor.h"

namespace filesync
{

SyncServerHandler::SyncServerHandler(reactor::Reactor& react)
  : sock_connection_handler(react)
  , localStoreDirectory_{boost::filesystem::current_path()}
  , receivedEntries_{}
{
  //file_reactor_ = new reactor::Reactor(new reactor::FileReactorImpl());
  //manager_.reset(new reactor::ConnectionManager(*file_reactor_));
}

int SyncServerHandler::handle_input(int handle)
{
  int ret = sock_connection_handler::handle_input(handle);
  if (ret < 0)
    return ret;

  while (input_buffer_.buffer_length() > sizeof(int64_t)) {
    auto lenParsed = decoder_.decode(input_buffer_);
    if (lenParsed <= 0) return -1;
    auto mes = decoder_.getMess();
    if (mes) {
      if (mes->header().command() == Command::ClientHello) {
        //LOG(INFO) << "Received client hello...";
        sayHello();
        //LOG(INFO) << "Send server hello...";
      }
      if (mes->header().command() == Command::DepositeFile) {
        LOG(INFO) << "Received deposite file mess...";
        LOG(INFO) << "File name: " << mes->header().depositefileheader().filename();
        LOG(INFO) << "File size: " << mes->header().depositefileheader().filelen();
        LOG(INFO) << "File content: " << mes->content();
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
