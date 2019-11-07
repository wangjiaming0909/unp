#include "gtest/gtest.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "net/inet_addr.h"
#include "examples/Downloader.h"

TEST(Downloader, geaddrinfo)
{
    addrinfo* addrs = nullptr;
    int ret = ::getaddrinfo("www.baidu.com", nullptr, nullptr, &addrs);

    if(addrs == nullptr)
    {
        std::cout << "didn't get addrinfo: " << strerror(errno) << std::endl;
        FAIL();
    }

    auto *ap = &addrs[0];

    if(ap)
    {
        sockaddr_in* inaddr = (sockaddr_in*)(ap->ai_addr);
        net::inet_addr add{*inaddr};
        std::cout << add.get_address_string() << std::endl;
        ap = ap->ai_next;
    }

    freeaddrinfo(addrs);
    SUCCEED();
}

TEST(Downloader, RetriveAddrFromUrl)
{
    examples::Downloader downloader{"https://www.baidupcs.com"};
    downloader.retriveAddrFromUrl();
    ASSERT_TRUE(downloader.targetAddr_.get_port_number() == 443);
    ASSERT_TRUE(downloader.targetAddr_.get_address_string().size() > 6);
    LOG(INFO) << downloader.targetAddr_.get_address_string();
}

TEST(Downloader, GetFileInfo)
{
    examples::Downloader downloader{""};
    auto ret = downloader.getFileInfo();
    ASSERT_TRUE(ret == -1);
}

TEST(Downloader, GetFileInfo2)
{
    GTEST_SKIP();
    examples::Downloader d{"https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz"};
    // examples::Downloader d{"https://github.com/wangjiaming0909/unp/archive/master.zip"};
    auto ret = d.getFileInfo();
    // ASSERT_TRUE(d.fileName_ == "boost_1_71_0.tar.gz");
    ASSERT_EQ(ret , 0);
    ret = d.download();
    ASSERT_EQ(ret , 0);
}