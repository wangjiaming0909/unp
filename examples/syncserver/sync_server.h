#pragma once
#include "reactor/tcp_server.h"
#include "syncserver/sync_server_handler.h"

namespace filesync
{
using SyncServer = reactor::tcp_server<SyncServerHandler>;
}