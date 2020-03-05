#include "net/inet_addr.h"
#include "net/unp.h"
#include "util/easylogging++.h"
#include "main_helper.h"
#include "downloader/download.h"
#include "downloader/serverhandler.h"
#include "reactor/tcp_server.h"
#include "reactor/echo_connection_handler.h"
#include <string>
#include "downloader/downloaderserver.h"
//#include "examples/Downloader.h"

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

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

void downloaderServer()
{

    using namespace std::chrono_literals;
    using namespace downloader;
    net::inet_addr listenArrd{8000, "0.0.0.0"};
    std::string url = "https://github.com/wangjiaming0909/unp/archive/master.zip";
    DownloaderServer server{listenArrd};;

    std::thread st{&DownloaderServer::start, &server};

    st.join();
}

int main(int argc, char** argv)
{
    server_scoped_helper s_h{argc, argv};
    //return download(argc, argv);
    // return serve(argc, argv);
    downloaderServer();
}

