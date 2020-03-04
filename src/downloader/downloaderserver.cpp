#include "downloaderserver.h"
#include "net/inet_addr.h"
#include "d.h"


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
    server_.set_thread_num(1);
}

int DownloaderServer::start()
{
    Pool::pool = new thread::thread_pool{4};
    Pool::pool->start();
    server_.start(unp::reactor_imp_t_enum::USING_EPOLL);
    return 0;
}

int DownloaderServer::stop()
{
    Pool::pool->cancel();
    delete Pool::pool;
    return server_.stop(true);
}

}
