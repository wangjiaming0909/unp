#include "net/inet_addr.h"
#include "net/unp.h"
#include "util/easylogging++.h"
#include "main_helper.h"
#include "downloader/download.h"
#include "downloader/serverhandler.h"
#include "reactor/tcp_server.h"
#include "reactor/echo_connection_handler.h"
#include <string>
//#include "examples/Downloader.h"

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int download(int argc, char** argv)
{
    if(argc < 2)
    {
        LOG(ERROR) << "a.out url";
        return -1;
    }
    std::string url = argv[1];
    downloader::Download d{url};
    int ret = d.download();
    std::cout << ret;
    return ret;
}

int serve(int argc, char** argv)
{
    if (argc < 3)
    {
        LOG(ERROR) << "a.out ip port";
        return -1;
    }
    const char* addr = argv[1];
    in_port_t port = std::stoi(argv[2]);
    net::inet_addr lisAddr{port, addr};
    reactor::tcp_server<downloader::DownloaderServerHandler> server{lisAddr};
    return server.start(unp::reactor_imp_t_enum::USING_EPOLL);
}

int main(int argc, char** argv)
{
    server_scoped_helper s_h{argc, argv};
    //return download(argc, argv);
    return serve(argc, argv);
}


