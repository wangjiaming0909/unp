#pragma once
#include <chrono>
#include <filesystem>
#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <map>

namespace filesync
{
    using DirE_t = std::filesystem::directory_entry;

class SyncEntryProperty
{
public:
    SyncEntryProperty(bool needSync = false);

public:
    bool needSync() const {return needSync_;}
    bool isSyncing() const {return isSyncing_;}
    void setNeedSync() {needSync_ = true;}
    void setIsSyncing() {isSyncing_ = true;}

private:
    bool needSync_;
    bool isSyncing_;
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
	explicit DirObservable(const std::filesystem::path& path);
    virtual ~DirObservable();

public:
	virtual int subscribe(std::shared_ptr<IDirObserver> observer) override;
	virtual void unsubscribe(int id, std::shared_ptr<IDirObservable> observer) override;
	void startObserveAsync(const std::atomic_int8_t& cancelToken);
    void stopObserve();
    EntryMap& entries() {return entries_;}

private:
    void startObserve(const std::atomic_int8_t& cancelToken);
    EntryMap addedEntries(const EntryMap& es1, const EntryMap& es2);
    EntryMap getEntriesOfDir(const Entry& dir);


private:
	Entry dir_;
	EntryMap entries_;

	std::unordered_map<int, std::weak_ptr<IDirObserver>> observers_;
    std::shared_ptr<std::thread> observeThread_;

    std::atomic_int8_t isMonitoring_;
    static const std::chrono::seconds syncInterval_;
};

}
