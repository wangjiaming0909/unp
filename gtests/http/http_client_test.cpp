#include <gtest/gtest.h>
#include "http/HttpClient.h"
#include "reactor/reactor.h"


class FakeHttpHandler : public http::HttpHandler
{
public:
  FakeHttpHandler(reactor::Reactor& react, bool is_ssl, int a)
    : http::HttpHandler(react, is_ssl)
  {}
  
  virtual int on_connected() override
  {}
  virtual int on_closed() override
  {
  }

};

TEST(http_client, normal)
{
  http::HttpClient client;
  client.get<FakeHttpHandler>("http://www.baidu.com", 1);
}
