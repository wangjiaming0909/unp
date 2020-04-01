#include <atomic>
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
            if(e.second.needSync() && e.second.isExisted())
            {
                LOG(INFO) << e.first.path().string() << " need sync...";
            }
            else if(!e.second.isExisted())
            {
              LOG(INFO) << e.first.path().string() << "deleted ...";
            }
        }
    }
};

TEST(DirMonitor, normal)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    filesync::DirObservable observable(p);
    auto& entriesExisted = observable.entries();

    std::atomic_int cancelToken;
    cancelToken.store(false);

    std::shared_ptr<filesync::IDirObserver> observer = std::make_shared<FakeDirObserver>();
    observable.subscribe(observer);
    observable.startObserveAsync(cancelToken);

    std::this_thread::sleep_for(400000s);
    boost::filesystem::path folder123;
    std::string path123 = "/home/jiaming/Music/123";
    folder123.assign(path123.begin(), path123.end());
    filesync::DirE_t e{folder123};
    filesync::SyncEntryProperty property{true};
    entriesExisted[e] = property;

    std::this_thread::sleep_for(10s);

    cancelToken = true;
    LOG(INFO) << "observe canceled...";

    std::this_thread::sleep_for(10s);
    observable.stopObserve();
}
