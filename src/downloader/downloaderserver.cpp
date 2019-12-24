#include "downloaderserver.h"
#include "net/inet_addr.h"


namespace downloader
{

DownloaderServer::DownloaderServer(net::inet_addr& listenAddr)
    : listenAddr_{listenAddr}
    , server_{listenAddr_}
    , ids_{}
    , downloadingQueue_{}
    , pausedQueue_{}
    , finishedQueue_{}
{
}


}
