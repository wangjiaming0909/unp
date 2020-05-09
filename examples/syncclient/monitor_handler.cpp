#include "syncclient/monitor_handler.h"
#include "boost/filesystem/operations.hpp"
#include "proto/decoder.h"


namespace filesync
{

FileMonitorHandler::FileMonitorHandler(reactor::Reactor& react, IDirObservable& observable) 
    : reactor::connection_handler(react), IDirObserver(observable)
{ }

void FileMonitorHandler::onUpdate(const EntryMap& es)
{
  for(auto& pair : es) {
    if (pair.second.needSync()) {
      auto& d_e = pair.first;
      auto fileSize = boost::filesystem::file_size(d_e.path());
      void* d = (void*)"a";
      auto package = getDepositeFilePackage(d_e.path().c_str(), fileSize, 0, 1, d);

      //mutex
      int64_t size = package->ByteSizeLong();
      char* data = static_cast<char*>(::calloc(size, 1));
      package->SerializeToArray(data, size);
      auto bytesWritten = write(size, false);
      bytesWritten += write(data, size, true);
      free(data);
    }
  }
}

int FileMonitorHandler::handle_close(int)
{
  return unsubscribe();
}

ServerMonitorHandler::ServerMonitorHandler(reactor::Reactor& react) 
    : reactor::connection_handler(react)
    , decoder_{}
{ }

int ServerMonitorHandler::open()
{
    return this->enable_reading();
}

int ServerMonitorHandler::handle_input(int handle)
{
  if (connection_handler::handle_input(handle) < 0) {
    return -1;
  }

  while (input_buffer_.total_len() > sizeof(int64_t)) {
    auto lenParsed = decoder_.decode(input_buffer_);
    if (lenParsed <= 0) return -1;
    auto mes = decoder_.getMess();
    if (mes) {
      if (mes->header().command() == Command::ServerHello) {
        LOG(INFO) << "Received server hello...";
        serverStatus_ = ServerStatus::connected;
      }
    }
    decoder_.reset();
  }
  return 0;
}
}
