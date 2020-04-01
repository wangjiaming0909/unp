#include "DirMonitor.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include "boost/filesystem/directory.hpp"
#include "util/easylogging++.h"

namespace filesync
{

SyncEntryProperty::SyncEntryProperty(bool needSync)
    : needSync_(needSync)
    , isSyncing_(false)
    , isExisted_(true)
{

}

using namespace std::chrono_literals;
const std::chrono::seconds DirObservable::syncInterval_ = 1s;

DirObservable::DirObservable(const Path_t& path)
    : dir_(path)
    , entries_{}
    , observers_{}
    , observeThread_{nullptr}
    , isMonitoring_(false)
{
    getEntriesOfDir(dir_, &entries_);
}

DirObservable::~DirObservable(){}

void DirObservable::startObserve(const std::atomic_int& cancelToken)
{
    while(!cancelToken)
    {
        auto addedAndDeletedEntries = monitorDir();
        if(addedAndDeletedEntries.first.size() > 0)
        {
            for(auto& observer : observers_)
            {
                if(observer.second.expired()) continue; 
                std::shared_ptr<IDirObserver> ob{observer.second};
                ob->onUpdate(addedAndDeletedEntries.first);
            }
        }

        if(addedAndDeletedEntries.second.size() > 0)
        {
            for(auto& observer : observers_)
            {
                if(observer.second.expired()) continue;
                std::shared_ptr<IDirObserver> ob{observer.second};
                ob->onUpdate(addedAndDeletedEntries.second);
            }
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

void DirObservable::startObserveAsync(const std::atomic_int& cancelToken)
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

EntryMap DirObservable::getEntriesOfDir(const Entry& dir, EntryMap* entryMap)
{
    EntryMap eMap{};
    if(entryMap != nullptr) 
    {
        for(auto& e : boost::filesystem::recursive_directory_iterator(dir.path()))
        {
            if(e == dir_) continue;
            entryMap->emplace(e, SyncEntryProperty(true));
        }
        return eMap;
    }
    else 
    {
        for(auto& e : boost::filesystem::recursive_directory_iterator(dir.path()))
        {
            if(e == dir_) continue;
            eMap.emplace(e, SyncEntryProperty(true));
        }
        return eMap;
    }
}

std::pair<EntryMap, EntryMap> DirObservable::monitorDir()
{
    EntryMap addedEntries{};
    EntryMap deletedEntries = entries_;

    for(auto& e : boost::filesystem::recursive_directory_iterator(dir_.path()))
    {
        if (e == dir_) continue;
        if (entries_.count(e) != 0 && !entries_[e].isExisted())//deleted once
        {
            entries_[e].setIsExisted(true);
            addedEntries.emplace(e, entries_[e]);
            continue;
        }
        if (entries_.count(e) != 0 && entries_[e].isExisted()) //existed already
        {
            deletedEntries.erase(e);
            continue;
        }
        if (entries_.count(e) == 0)//new added
        {
            addedEntries.emplace(e, SyncEntryProperty(true));
            entries_.emplace(e, SyncEntryProperty(true));//update entries_ also
            continue;
        }
    }

    EntryMap newDeletedEntries{};
    for(auto& e : deletedEntries)
    {
        auto& property = entries_[e.first];
        if (property.isExisted()) 
        {
            property.setIsExisted(false);
            newDeletedEntries.emplace(e.first, property);
        }
    }

    return {addedEntries, newDeletedEntries};
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
