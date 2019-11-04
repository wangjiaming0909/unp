#include <gtest/gtest.h>
#include <thread>
#include "reactor/tcp_client.h"
#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"
#include "examples/HttpClientHandler.h"
#include "examples/HttpsClientHandler.h"
#include "util/easylogging++.h"

TEST(tcp_client, normal)
{
    GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_SELECT};
    // using Connector_t = connector<echo_client_handler>;
    using Connector_t = connector<examples::HttpClientHandler>;


    // net::inet_addr target_addr{22, "127.0.0.1"};
    // net::inet_addr target_addr{9090, "192.168.0.2"};
    net::inet_addr target_addr{80, "61.135.169.121"};
    // net::inet_addr target_addr{80, "104.193.88.77"};
// 

    std::vector<Connector_t*> conns{};

    const char* url = "http://www.baidu.com";
    // char* userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux) Gecko/20100101 Firefox/61.0";
    const char* userAgent = "curl/7.47.0";

    int connCount = 1;
    for(int i = 0; i < connCount; i++)
    {
        auto* conn = client.addConnection<Connector_t>(url, userAgent, "name");
        ASSERT_TRUE(conn != nullptr);
        conns.push_back(conn);
        auto* handler = conn->connect(target_addr, 2s);
        if(handler == nullptr) FAIL();
    }

    std::thread th1{&tcp_client::start, &client};

    // std::this_thread::sleep_for(10s);
    // for(int i = 0; i < connCount; i++)
    // {
    //     conns[i]->disconnect(2s);
    // }
    // std::this_thread::sleep_for(4s);
    // client.suspend();

    th1.join();
}

TEST(tcp_client, Get_SHANGZHENGZHISHU)
{
    GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_EPOLL};
    using Connector_t = connector<examples::HttpClientHandler>;

    net::inet_addr target_addr1{80, "119.97.156.107"};
    net::inet_addr target_addr2{80, "61.129.248.228"};
    net::inet_addr target_addr3{80, "61.129.248.228"};

    std::vector<Connector_t*> conns{};

    const char* url_SZZS = "http://stockpage.10jqka.com.cn/1A0001/quote/header/";
    const char* url_JY_JJ = "http://fundgz.1234567.com.cn/js/519712.js?rt=1566883474043";
    const char* url_YFD_JJ = "http://fundgz.1234567.com.cn/js/110011.js?rt=1566883751040";
    const char* userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux) Gecko/20100101 Firefox/61.0";
    // char* userAgent = "curl/7.47.0";

    auto lambda = [&client, &conns](const char* url, const char* userAgent, net::inet_addr& addr, std::string& name)
    {
        while(true)
        {
            auto* conn = client.addConnection<Connector_t>(url, userAgent, name);
            ASSERT_TRUE(conn != nullptr);
            conns.push_back(conn);
            auto* handler = conn->connect(addr, 2s);
            if(handler == nullptr) FAIL();

            auto ret = client.start();
            client.closeConnection<Connector_t>(*conn, 1s);
            ASSERT_TRUE(ret < 0);
            std::this_thread::sleep_for(5s);
        }
    };

    std::string name1 = "上证指数";
    std::string name2 = "交银阿尔法";
    std::string name3 = "易方达";

    auto run = [lambda, url_JY_JJ, url_SZZS, url_YFD_JJ, userAgent, &target_addr1, &target_addr2, &target_addr3, &name1, &name2, &name3]()
    {
        lambda(url_SZZS, userAgent, target_addr1, name1);
        // lambda(url_JY_JJ, userAgent, target_addr2, name2);
        // lambda(url_YFD_JJ, userAgent, target_addr3, name3);
    };

    // std::thread th1{&tcp_client::start, &client};
    std::thread th{run};

    th.join();
}

TEST(httpDownloader, normal)
{
    GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_EPOLL};
    using Connector_t = connector<examples::HttpDownloader>;

    // net::inet_addr target_addr1{80, "192.168.0.2"};
    // net::inet_addr target_addr1{80, "127.0.0.1"};
    net::inet_addr target_addr1{8080, "163.184.56.140"};

    std::string name = "downloader";

    std::vector<Connector_t*> conns{};

    // const char* url = "http://192.168.0.2/home/pi/m/1/1.mkv";
    // const char* url = "http://192.168.0.2/home/pi/Documents/cert/fd.cnf";
    // const char* url = "http://163.184.56.140/home/jwang284/downloads/1/bootstrap.log";
    const char* url = "http://163.184.56.140:8080/home/jwang284/downloads/sqlite.tar";
    const char* userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux) Gecko/20100101 Firefox/61.0";
    // char* userAgent = "curl/7.47.0";

    auto lambda = [&client, &conns](const char* url, const char* userAgent, net::inet_addr& addr, std::string& name)
    {
        auto* conn = client.addConnection<Connector_t>(url, userAgent, name);
        ASSERT_TRUE(conn != nullptr);
        conns.push_back(conn);
        auto* handler = conn->connect(addr, 2s);
        if(handler == nullptr) FAIL();

        auto ret = client.start();
        client.closeConnection<Connector_t>(*conn, 1s);
        ASSERT_TRUE(ret < 0);
    };


    auto run = [&]()
    {
        lambda(url, userAgent, target_addr1, name);
    };

    std::thread th{run};

    th.join();
}

static string_piece::const_string_piece USERAGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:71.0) Gecko/20100101 Firefox/71.0";
static string_piece::const_string_piece ACCEPT = "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
static string_piece::const_string_piece ACCEPTENCODING = "gzip, deflate, br";
static string_piece::const_string_piece ACCEPTLANGUAGE = "en-US,en;q=0.5";
static string_piece::const_string_piece CONNECTION = "keep-alive";
static string_piece::const_string_piece UPGRADE_INSECURE_REQUEST = "1";
static const char* URL = "https://d11.baidupcs.com/file/cde7a2bffa229895da002ee6eb968d35?bkt=en-4d166c071887761595f0a1dcc28cd1950e766ac18491a7067532c0dd8c3ef963d120e56d7151fda3&xcode=dd3ed8191fcfbed0be28c2ebf14da789017f1bc0040e0d03587371082fe524e9c830214ff6ea44bfc1f7447f079d73159717ec4418c70769&fid=556088867-250528-1074812157753092&time=1572859687&sign=FDTAXGERLQBHSKfa-DCb740ccc5511e5e8fedcff06b081203-YC9cbIDWF9a50NxgfuecM%2FnVvR0%3D&to=d11&size=1048576000&sta_dx=1048576000&sta_cs=6206&sta_ft=rar&sta_ct=3&sta_mt=3&fm2=MH%2CYangquan%2CAnywhere%2C%2Cguangdong%2Cother&ctime=1571976868&mtime=1572019681&resv0=cdnback&resv1=0&resv2=&resv3=&resv4=1048576000&vuk=556088867&iv=0&htype=&randtype=&newver=1&newfm=1&secfm=1&flow_ver=3&pkey=en-68081b6cfceb6734477e9a421b43b956c8fe9e4237effe799fbf90cddb1065889b5bac86bae5c493&sl=76480590&expires=8h&rt=pr&r=548715997&mlogid=7137077980861615109&vbdid=4209523675&fin=3DMGAME-The.Outer.Worlds.Cracked-3DM.part23.rar&fn=3DMGAME-The.Outer.Worlds.Cracked-3DM.part23.rar&rtype=1&dp-logid=7137077980861615109&dp-callid=0.1.1&hps=1&tsl=80&csl=80&csign=ddiWqcmxmeFiDxTFrH9VH%2FlO3tY%3D&so=0&ut=6&uter=4&serv=0&uc=2392251241&ti=8525e99dbc6685b14e16db9273b3241002c09a90630ffbdf&reqlabel=250528_d&by=themis";

int SetupRequest(http::HttpMessage& mes)
{
    mes.setHttpVersion(1, 1);
    mes.setRequestMethod(http::HTTPMethod::GET);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_USER_AGENT, USERAGENT);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT, ACCEPT);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING, ACCEPTENCODING);
    mes.addHeader(http::HttpHeaderCode::HTTP_HEADER_CONNECTION, CONNECTION);
    return 0;
}

TEST(HttpsClientHandler, normal)
{
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_EPOLL};
    using Connector_t = connector<examples::HttpsClientHandler>;

    // net::inet_addr target_addr1{443, "61.135.169.121"};
    net::inet_addr target_addr1{443, "119.146.74.46"};

    std::string name = "downloader";

    std::vector<Connector_t*> conns{};

    // const char *url = "https://www.baidu.com";
    // const char *url = "https://www.baidupcs.com/rest/2.0/pcs/file?method=batchdownload&app_id=250528&zipcontent=%7B%22fs_id%22%3A%5B992917886813383%5D%7D&sign=DCb740ccc5511e5e8fedcff06b081203:GZfvNUxNdjYxcXjFJFcZx5%2Fb5Ps%3D&uid=556088867&time=1572860808&dp-logid=7114186001274374425&dp-callid=0&vuk=556088867&zipname=3DMGAME-The.Outer.Worlds.Cracked-3DM.part01.zip";

    auto lambda = [&client, &conns](const char* url, net::inet_addr& addr)
    {
        auto* conn = client.addConnection<Connector_t>(url, std::move(SetupRequest));
        ASSERT_TRUE(conn != nullptr);
        conns.push_back(conn);
        auto* handler = conn->connect(addr, 2s);
        if(handler == nullptr) 
        {
            LOG(WARNING) << "connection error..." ;
        }

        auto ret = client.start();
        client.closeConnection<Connector_t>(*conn, 1s);
        ASSERT_TRUE(ret < 0);
    };


    auto run = [&]()
    {
        lambda(URL, target_addr1);
    };

    std::thread th{run};

    th.join();

}
