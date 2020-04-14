#include <iostream>
#include "reactor/tcp_server.h"
#include "util/easylogging++.h"
#include "syncserver/sync_server.h"
#include "sync_server_handler.h"
using namespace std;
using namespace filesync;

INITIALIZE_EASYLOGGINGPP

int main()
{
    net::inet_addr listenAddr{9090, "127.0.0.1"};
    SyncServer server{listenAddr};
    server.start(unp::reactor_imp_t_enum::USING_EPOLL);
    cout << 123 << endl;
}
