#include "syncserver/sync_server_handler.h"
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
  LOG(DEBUG) << "handle_input returned: " << handle;
  if (ret < 0)
    return ret;

  while (input_buffer_.buffer_length() > sizeof(int64_t)) {
    LOG(DEBUG) << "before deocde buffer len: " << input_buffer_.buffer_length();
    auto lenParsed = decoder_.decode(input_buffer_);
    LOG(DEBUG) << "SyncServerHandler::handle_input decode returned: " << lenParsed;
    if (lenParsed <= 0) {
      if (decoder_.isError())
        return -1;
    }
    if (decoder_.isCompleted()) {
      auto mes = decoder_.getMess();
      if (mes) {
        if (mes->header().command() == Command::ClientHello) {
          LOG(DEBUG) << "Client hello message: " << "aaaaaaaaaaaaaaaaaaaaaa";//mes->content();
          LOG(INFO) << "Received client hello...";
          sayHello();
          //LOG(INFO) << "Send server hello...";
        } else if (mes->header().command() == Command::DepositeFile) {
          LOG(INFO) << "Received deposite file mess...";
          LOG(INFO) << "File name: " << mes->header().depositefileheader().filename();
          LOG(INFO) << "File size: " << mes->header().depositefileheader().filelen();
          LOG(INFO) << "File from: " << mes->header().depositefileheader().curseqstart();
          LOG(INFO) << "File to: " << mes->header().depositefileheader().curseqend();
        }
      }
      LOG(DEBUG) << "after decode buffer len: " << input_buffer_.buffer_length();
    }
    if (lenParsed == 0)// && !decoder_.isCompleted())
      break;
  }
  LOG(DEBUG) << "SyncServerHandler returned, input_buffer_ size: " << input_buffer_.total_len();
  return 0;
}

void SyncServerHandler::sayHello()
{
  static char* d = (char*)::calloc(1, 10240);
  memset(d, 98, 10239);
  const char* hello = d;
  auto helloPackage = getHelloPackage(hello, PackageType::Server);
  sendPackage(helloPackage);
}
}
