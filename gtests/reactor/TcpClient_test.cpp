#include <gtest/gtest.h>
#include <thread>
#include "reactor/tcp_client.h"
#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"
#include "examples/HttpClientHandler.h"
#include "examples/HttpsClientHandler.h"

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

TEST(HttpsClientHandler, normal)
{
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_EPOLL};
    using Connector_t = connector<examples::HttpsClientHandler>;

    // net::inet_addr target_addr1{443, "61.135.169.121"};
    net::inet_addr target_addr1{443, "119.167.143.64"};

    std::string name = "downloader";

    std::vector<Connector_t*> conns{};

    // const char *url = "https://www.baidu.com";
    const char *url = "https://www.baidupcs.com/rest/2.0/pcs/file?method=batchdownload&app_id=250528&zipcontent=%7B%22fs_id%22%3A%5B992917886813383%5D%7D&sign=DCb740ccc5511e5e8fedcff06b081203:GZfvNUxNdjYxcXjFJFcZx5%2Fb5Ps%3D&uid=556088867&time=1572860808&dp-logid=7114186001274374425&dp-callid=0&vuk=556088867&zipname=3DMGAME-The.Outer.Worlds.Cracked-3DM.part01.zip";

    auto lambda = [&client, &conns](const char* url, net::inet_addr& addr)
    {
        auto* conn = client.addConnection<Connector_t>(url);
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
        lambda(url, target_addr1);
    };

    std::thread th{run};

    th.join();

}
