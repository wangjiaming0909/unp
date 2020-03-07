#include "DirMonitor.h"
#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <thread>
#include "util/easylogging++.h"

namespace filesync
{

SyncEntryProperty::SyncEntryProperty(bool needSync)
    : needSync_(needSync)
    , isSyncing_(false)
{

}

using namespace std::chrono_literals;
const std::chrono::seconds DirObservable::syncInterval_ = 2s;

DirObservable::DirObservable(const std::filesystem::path& path)
    : dir_(path)
    , entries_{}
    , observers_{}
    , observeThread_{nullptr}
    , isMonitoring_(false)
{
    auto es = getEntriesOfDir(dir_);
    entries_.insert(es.begin(), es.end());
}

DirObservable::~DirObservable(){}

void DirObservable::startObserve(const std::atomic_int8_t& cancelToken)
{
    while(!cancelToken)
    {
        auto es = getEntriesOfDir(dir_);
        auto addedEs = addedEntries(es, entries_);
        if(entries_.size() > 0)
        {
            for(auto& observer : observers_)
            {
                if(observer.second.expired()) continue;
                std::shared_ptr<IDirObserver> ob{observer.second};
                ob->onUpdate(entries_);
            }
            for(auto& pair : entries_)
            {
                pair.second.setIsSyncing();
            }
            entries_.insert(addedEs.begin(), addedEs.end());
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(syncInterval_);
    }
    isMonitoring_ = false;
}

void DirObservable::stopObserve()
{
    if(observeThread_ && observeThread_->joinable())
    {
        observeThread_->join();
        observeThread_.reset();
    }
}

void DirObservable::startObserveAsync(const std::atomic_int8_t& cancelToken)
{
    if(cancelToken) return;
    observeThread_.reset(new std::thread(&DirObservable::startObserve, this, std::ref(cancelToken)));
    isMonitoring_ = true;
}

EntryMap DirObservable::addedEntries(const EntryMap& esNew, const EntryMap& esOld)
{
    EntryMap addedEntries{};
    for(auto& e : esNew)
    {
        if(esOld.find(e.first) == esOld.end())
        {
            addedEntries.insert(e);
        }
    }
    return addedEntries;
}

EntryMap DirObservable::getEntriesOfDir(const Entry& dir)
{
    EntryMap eMap{};
    for(auto& e : std::filesystem::recursive_directory_iterator(dir.path()))
    {
        if(e == dir_) continue;
        eMap.emplace(e, SyncEntryProperty(true));
    }
    return eMap;
}

int DirObservable::subscribe(std::shared_ptr<IDirObserver> observer)
{
    if(isMonitoring_) return -1;
    int id = 0;
    while(observers_.count(id) != 0)
    {
        id++;
    }
    observers_[id] = observer;
    return 0;
}

void DirObservable::unsubscribe(int id, std::shared_ptr<IDirObservable>)
{
    if(observers_.count(id) == 0)
    {
        LOG(WARNING) << "not subscribed, can't unsunscribe id: " << id;
        return;
    }
    observers_.erase(id);
}

}
