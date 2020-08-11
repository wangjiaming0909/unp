#include "syncclient/monitor_handler.h"
#include "boost/filesystem/operations.hpp"
#include "examples/dirmonitor/DirMonitor.h"
#include "net/unp.h"
#include "proto/decoder.h"
#include "proto/sync_mess.pb.h"
#include "reactor/file_reactor_impl.h"
#include "util/easylogging++.h"
#include <mutex>


namespace filesync
{

FileMonitorHandler::FileMonitorHandler(reactor::Reactor& react, IDirObservable& observable)
  : reactor::sock_connection_handler(react)
  , IDirObserver(observable)
  , entries_()
  , pending_sync_set_()
  , syncing_set_()
  , finished_set_()
  , paused_set_()
  , error_set_()
  , pending_set_cv_()
  , reactor_(nullptr)
  , manager_(nullptr)
  , readers_()
  , readers_map_()
{
  reactor_ = new reactor::Reactor(new reactor::FileReactorImpl());
  manager_.reset(new reactor::ConnectionManager(*reactor_));
  unp::get_thread_pool(4)->add_task(
      [=](){
        this->sync();
      }
    );
}

void FileMonitorHandler::onUpdate(const EntryMap& es)
{
  for(auto& pair : es) {
    if (pair.second.needSync()) {
      if (!pair.second.isExisted())
        continue;
      add_to_need_sync(pair.first);
    }
  }
}

int FileMonitorHandler::handle_close(int)
{
  LOG(DEBUG) << "File Monitor Handler handle close...";
  return unsubscribe();
}

//add to need_sync_map and add to backend thread
void FileMonitorHandler::add_to_need_sync(const Entry& e)
{
  std::lock_guard<std::mutex> guard(pending_set_mutex_);
  if (error_set_.count(e)) {
    LOG(WARNING) << "Entry has been in the error set: " << e.path();
    return;
  }
  if (syncing_set_.count(e)) {
    LOG(WARNING) << "Entry has been in the syncing set: " << e.path();
    return;
  }
  if (paused_set_.count(e)) {
    LOG(DEBUG) << "Entry deleted from paused_set_: " << e.path();
    paused_set_.erase(e);
  }
  LOG(DEBUG) << "Entry add to syncing: " << e.path();
  if (!boost::filesystem::exists(e.path())) {
    LOG(WARNING) << "Adding a non existed entry: " << e.path();
    return;
  }
  pending_sync_set_.insert(e);
  auto file_size = boost::filesystem::file_size(e.path());
  auto file_name = e.path().string();
  auto *reader = manager_->makeConnection<ReaderType>(*this, file_name, static_cast<uint64_t>(file_size));
  reader->open_file(e.path().string().c_str(), O_RDONLY);
  readers_.push_back(reader);
  close_reader(file_name);
  readers_map_[file_name] = reader;
  pending_set_cv_.notify_one();
}

void FileMonitorHandler::sync()
{
  while(!cancel_reactor_token_) {
    std::unique_lock<std::mutex> lock(pending_set_mutex_, std::defer_lock);
    lock.lock();
    while (pending_sync_set_.empty())
      pending_set_cv_.wait(lock);
    lock.unlock();
    auto ret = reactor_->handle_events();
    if (ret < 0) {
      LOG(WARNING) << "FileMonitorHandler handle events returned -1";
      break;
    }
    LOG(DEBUG) << "FileMonitorHandler handle_events returned: " << ret;
  }
}

int FileMonitorHandler::add_to_finished(const Entry& e)
{
  std::lock_guard<std::mutex> guard(pending_set_mutex_);
  if (pending_sync_set_.count(e) == 0) {
    LOG(WARNING) << "Adding a not syncing entry to finished_set_: " << e.path().string();
    return -1;
  }
  LOG(DEBUG) << "Entry added to finished: " << e.path().string();
  pending_sync_set_.erase(e);
  finished_set_.insert(e);
  close_reader(e.path().string());
  return 0;
}


void FileMonitorHandler::close_reader(const std::string& file_name)
{
  if (readers_map_.count(file_name) == 1) {
    using namespace std::chrono_literals;
    manager_->closeConnection<ReaderType>(*readers_map_[file_name], 1s);
    readers_map_.erase(file_name);
  } else {
    LOG(WARNING) << "Trying to close a non exist reader: " << file_name;
  }
}

void FileMonitorHandler::add_to_pause(const Entry& e)
{
  if (syncing_set_.count(e) == 0) {
    LOG(WARNING) << "Pausing entry: " << e.path() << ", but it's not in the syncing set";
    return;
  }
  LOG(DEBUG) << "Entry add to paused: " << e.path();
  paused_set_.insert(e);
}

void FileMonitorHandler::add_to_error(const Entry& e)
{
  if (syncing_set_.count(e) == 0 && paused_set_.count(e) == 0) {
    LOG(WARNING) << "Pausing entry: " << e.path() << ", but it's not in the syncing and paused set";
    return;
  }
  LOG(DEBUG) << "Entry add to error: " << e.path();
  close_reader(e.path().string());
  error_set_.insert(e);
}

ServerMonitorHandler::ServerMonitorHandler(reactor::Reactor& react)
  : reactor::sock_connection_handler(react)
  , decoder_{}
{ }

int ServerMonitorHandler::open()
{
  return this->enable_reading();
}

int ServerMonitorHandler::handle_input(int handle)
{
  if (sock_connection_handler::handle_input(handle) < 0)
    return -1;

  while (input_buffer_.total_len() > sizeof(int64_t)) {
    auto lenParsed = decoder_.decode(input_buffer_);
    if (lenParsed <= 0) return -1;
    auto mes = decoder_.getMess();
    if (mes) {
      if (mes->header().command() == Command::ServerHello) {
        LOG(INFO) << "Server OK...";
        serverStatus_ = ServerStatus::connected;
      }
    }
    decoder_.reset();
  }
  return 0;
}
}
