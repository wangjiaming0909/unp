#include <iostream>
#include "net/inet_addr.h"
#include "util/easylogging++.h"
#include "syncclient/sync_client.h"
#include "main_helper.h"
#include "net/unp.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;
int main()
{
    setupLogger();
    unp::get_thread_pool(4);
    net::inet_addr target{9090, "127.0.0.1"};
    filesync::SyncClient client{target};
    client.setMonitorFolder("/home/wjm/codes/unp/build");
    std::atomic_int cancelToken{1};
    client.start(cancelToken);
    cout << 123 << endl;
}
