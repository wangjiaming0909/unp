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
static const char* URL = "https://d11.baidupcs.com/file/41f2622834d1f599f0f6a010d10ea602?bkt=en-e031c0692dcd5a21b966b62a84525b44d2eed44cceda0696c6e7c50ffbdfd37baf3ef490c836a2c6&xcode=54e303cfef53cc8e4b7929d185afeac15f3b9236f28064478cfff038b3ea7b3dc8de94d91b025bfa4fec6cf53b685d099717ec4418c70769&fid=3961905878-250528-186602276349228&time=1572869616&sign=FDTAXGERLQBHSKfa-DCb740ccc5511e5e8fedcff06b081203-fIILJH2WdkLEQg6DTgiVb4yu%2F90%3D&to=d11&size=1048576000&sta_dx=1048576000&sta_cs=63470&sta_ft=rar&sta_ct=6&sta_mt=6&fm2=MH%2CQingdao%2CAnywhere%2C%2Cbeijing%2Ccnc&ctime=1550679710&mtime=1550748065&resv0=cdnback&resv1=0&resv2=rlim&resv3=5&resv4=1048576000&vuk=3961905878&iv=0&htype=&randtype=&newver=1&newfm=1&secfm=1&flow_ver=3&pkey=en-2bac2b91d636e03bbcde6111567716da3c20894de8c98fba1fa42f2528bbc2bd08d7fd7b79d55129&sl=76480590&expires=8h&rt=pr&r=451136983&mlogid=7139743198834173732&vbdid=1471524624&fin=3DMGAME-Metro.Exodus.Gold.Edition.Cracked-CPY.part05.rar&fn=3DMGAME-Metro.Exodus.Gold.Edition.Cracked-CPY.part05.rar&rtype=1&dp-logid=7139743198834173732&dp-callid=0.1.1&hps=1&tsl=80&csl=80&csign=ln%2F67at3t7V0Axi5ddlmFKD7yc4%3D&so=0&ut=6&uter=4&serv=0&uc=2392251241&ti=76168191086d6f2977f801cdd0966ca4881fd3c13fc16aa9&reqlabel=250528_d&by=themis";

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
    GTEST_SKIP();
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
