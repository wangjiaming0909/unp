#include <atomic>
#include <c++/9.2.0/bits/fs_fwd.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include "examples/dirmonitor/DirMonitor.h"
#include "util/easylogging++.h"
using namespace std::chrono_literals;

struct FakeDirObserver : public filesync::IDirObserver
{
	virtual void onUpdate(const filesync::EntryMap& es) override
    {
        for(auto& e : es)
        {
            if(e.second.needSync() && !e.second.isSyncing())
            {
                LOG(INFO) << e.first.path().string() << " need sync...";
            }
        }
    }
};

TEST(DirMonitor, normal)
{
    std::filesystem::path p{};
    p.assign("/home/jiaming/Music");
    filesync::DirObservable observable(p);
    auto& entriesExisted = observable.entries();

    std::atomic_int8_t cancelToken = false; 

    std::shared_ptr<filesync::IDirObserver> observer = std::make_shared<FakeDirObserver>();
    observable.subscribe(observer);
    observable.startObserveAsync(cancelToken);

    std::this_thread::sleep_for(4s);
    std::filesystem::path folder123;
    folder123.assign("/home/jiaming/Music/123");
    filesync::DirE_t e{folder123};
    filesync::SyncEntryProperty property{true};
    entriesExisted[e] = property;

    std::this_thread::sleep_for(10s);

    cancelToken = true;
    LOG(INFO) << "observe canceled...";

    std::this_thread::sleep_for(10s);
    observable.stopObserve();
}
