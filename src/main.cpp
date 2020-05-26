#include "net/inet_addr.h"
#include "net/inet_sock_stream.h"
#include "net/sock_stream.h"
#include "net/unp.h"
#include "thread/thread_pool.h"
#include "util/easylogging++.h"
#include "main_helper.h"
#include "downloader/download.h"
#include "downloader/serverhandler.h"
#include "reactor/tcp_server.h"
#include "reactor/echo_connection_handler.h"
#include <cstdio>
#include <netdb.h>
#include <string>
#include "downloader/downloaderserver.h"
#include "util/timer.h"
#include <set>
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

void scan_(std::string str, thread::thread_pool* pool)
{
    for(int i = 1; i < 255; i++)
    {
      auto netStr = str;
      netStr.append(std::to_string(i));
      net::inet_addr targetAddr{22, netStr.c_str()};
      net::sock_connector connector{};
      net::InetSockStream stream{};
      std::chrono::microseconds timeout = 50ms;
      int ret = connector.connect(stream, targetAddr, &timeout, 1, 0);
      stream.close();
      if (ret != 0) 
      {
        continue;
      }
      LOG(INFO) << netStr << " connect succeed...";
      // std::string command = "sshpass -p \"123\" ssh wjm@" + netStr;
      // auto file = popen(command.c_str(), "rw");
      // char buffer[1024] = {};
      // auto c = ::fgets(buffer, 1024, file);
      // //std::cout << "buffer: " << buffer << std::endl;
      // if (strcmp(buffer, "Permission denied") != 0) continue;
      // LOG(INFO) << netStr << " ssh connect succeed ...";
      // pool->cancel();
    }
}

int scan()
{
  using namespace std::chrono_literals;
  thread::thread_pool pool{2};
  pool.start();
  //172.16.70.1
  for(int i = 70; i < 71; i++)
  {
    std::string str = "172.16.";
    str = str + std::to_string(i) + ".";
    pool.add_task(std::bind(scan_, str, &pool));
  }
  pool.wait(nullptr);
  return -1;
}

void test_plus()
{
  using namespace std;
  vector<int> v;
  for (int i = 0; i < 10000000; i++)
  {
    v.push_back(i);
  }

  auto it = v.begin();
  {
    utils::timer _{"it++"};
    for(; it != v.end(); it++)
    {

    }
  }
  it = v.begin();
  {
    utils::timer _{"++it"};
    for(; it != v.end(); ++it)
    {

    }
  }
}

int main(int argc, char** argv)
{
    //server_scoped_helper s_h{argc, argv};
    //return download(argc, argv);
    // return serve(argc, argv);
    //downloaderServer();
    //
    //scan();
    //test_plus();
    //
  std::set<string> s{};
  s.insert("asd1");
  s.insert("asd2");
  s.insert("asd3");
  s.insert("asd4");
  s.insert("asd5");

  auto it = s.begin();
  bool b = 1;
  for(; it != s.end(); ++it)
  {
    if (b)
    {
      auto it2 = s.begin();
      s.erase(++it2);
    }
  }
}


