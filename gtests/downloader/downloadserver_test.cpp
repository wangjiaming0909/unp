#include <cstring>
#include <gtest/gtest.h>
#include <downloader/downloaderserver.h>
#include <thread>
#include <string.h>

#include "net/inet_addr.h"
#include "net/unp.h"
#include "proto/mess.pb.h"
#include "reactor/buffer.h"
#include "reactor/connection_handler.h"
#include "reactor/connector.h"
#include "reactor/reactor.h"
#include "reactor/tcp_client.h"
#include "util/easylogging++.h"

using namespace downloader;

struct ProtoTestHandler : public reactor::connection_handler
{
public:
    ProtoTestHandler(reactor::Reactor& react, const std::string& url) 
        : connection_handler(react)
        ,url_(url) {}

    virtual int handle_input(int handle) override
    {
        if(connection_handler::handle_input(handle) < 0)
        {
            return -1;
        }
        char* d = (char*)::calloc(1024, 1);
        input_buffer_.read_line(d, 1024, reactor::buffer_eol_style::BUFFER_EOL_LF);
        const char* finishedMess = "F";
        if (::memcmp(d, finishedMess, strlen(finishedMess)) == 0)
        {
            LOG(INFO) << "download finsihed...";
            return closeHandler();
        }
        return 0;
    }

    int closeHandler()
    {
        return -1;
    }


    virtual int open() override
    {
        using namespace downloadmessage;
        Mess_WL mes{};
        mes.set_id(1);
        mes.set_command(Mess_WL::DownloadCommand::Mess_WL_DownloadCommand_DOWNLOAD);
        mes.set_url(url_);
        auto len = sizeof(1)
            + sizeof(Mess_WL::DownloadCommand)
            + sizeof(int32_t)
            + url_.size();
        mes.set_len(len);

        char* d = (char*)::calloc(1024, 1);
        mes.SerializeToArray(d, 1024);
        auto bytesWritten = write(d, 1024);
        free(d);
        LOG(INFO) << "mes len: " << len;
        LOG(INFO) << "bytes writen: " << bytesWritten;
        enable_reading();
        return 0;
    }

    std::string url_;
};


TEST(downloadserver, normal)
{
    using namespace std::chrono_literals;
    net::inet_addr listenArrd{8000, "0.0.0.0"};
    std::string url = "https://github.com/wangjiaming0909/ace_fork/archive/master.zip";
    DownloaderServer server{listenArrd};;

    std::thread st{&DownloaderServer::start, &server};

    //client
    std::this_thread::sleep_for(2s);
    reactor::tcp_client client{unp::reactor_imp_t_enum::USING_EPOLL};
    using Connector_t = reactor::connector<ProtoTestHandler>;
    auto connector = client.addConnection<Connector_t>(url);
    net::inet_addr targetAddr{8000, "163.184.56.140"};

    auto connection = connector->connect(targetAddr, 2s);

    auto ret = client.start();
    LOG(INFO) << "client ret: " << ret;

    client.closeConnection<Connector_t>(*connector, 2s);

    st.join();
}
