#include "downloaderserver.h"
#include "net/inet_addr.h"


namespace downloader
{

DownloaderServer::DownloaderServer(net::inet_addr& listenAddr)
    : listenAddr_{listenAddr}
    , server_{listenAddr_}
    // , ids_{}
    // , downloadingQueue_{}
    // , pausedQueue_{}
    // , finishedQueue_{}
    // , partialMesses_{}
{
}


int DownloaderServer::start()
{
    server_.start(unp::reactor_imp_t_enum::USING_EPOLL);
}

}
