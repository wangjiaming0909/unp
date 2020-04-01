#pragma once
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <map>

#include "boost/filesystem.hpp"

namespace filesync
{
    using DirE_t = boost::filesystem::directory_entry;
    using Path_t = boost::filesystem::path;

class SyncEntryProperty
{
public:
    SyncEntryProperty(bool needSync = false);

public:
    bool needSync() const {return needSync_;}
    bool isSyncing() const {return isSyncing_;}
    void setNeedSync() {needSync_ = true;}
    void setIsSyncing() {isSyncing_ = true;}
    bool isExisted() const {return isExisted_;}
    void setIsExisted(bool b) {isExisted_ = b;}

private:
    bool needSync_;
    bool isSyncing_;
    bool isExisted_;
};

using Entry = DirE_t;
using EntryVector = std::vector<Entry>;
using EntryMap = std::map<Entry, SyncEntryProperty>;

struct IDirObserver
{
	virtual void onUpdate(const EntryMap& es) = 0;
};

struct IDirObservable
{
	virtual int subscribe(std::shared_ptr<IDirObserver> observer) = 0;
	virtual void unsubscribe(int id, std::shared_ptr<IDirObservable> observer) = 0;
};

class DirObservable : public IDirObservable
{
public:
	explicit DirObservable(const Path_t& path);
  virtual ~DirObservable();

public:
	virtual int subscribe(std::shared_ptr<IDirObserver> observer) override;
	virtual void unsubscribe(int id, std::shared_ptr<IDirObservable> observer) override;
	void startObserveAsync(const std::atomic_int& cancelToken);
  void stopObserve();
  EntryMap& entries() {return entries_;}

private:
    void startObserve(const std::atomic_int& cancelToken);
    EntryMap addedEntries(const EntryMap& es1, const EntryMap& es2);
    EntryMap getEntriesOfDir(const Entry& dir, EntryMap* entryMap);
    std::pair<EntryMap, EntryMap> monitorDir();


private:
	Entry dir_;
	EntryMap entries_;

	std::unordered_map<int, std::weak_ptr<IDirObserver>> observers_;
  std::shared_ptr<std::thread> observeThread_;

  std::atomic_int isMonitoring_;
  static const std::chrono::seconds syncInterval_;
};

}
