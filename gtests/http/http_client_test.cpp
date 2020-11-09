#include <gtest/gtest.h>
#include "http/HttpClient.h"
#include "http/Http1xCodec.h"
#include "reactor/reactor.h"
#include "util/easylogging++.h"


class FakeHttpHandler : public http::HttpHandler
{
public:
  FakeHttpHandler(reactor::Reactor& react, bool is_ssl, int a)
    : http::HttpHandler(react, is_ssl)
  {}

  virtual int on_connected() override
  {
    return 0;
  }
  virtual int on_closed() override
  {
    return 0;
  }

  virtual int onStatus(const char*, size_t) override
  {
    LOG(INFO) << "on status: " << codec_->status();
    if (codec_->status() == 302) {
      return -1;
    }
    return 0;
  }

  virtual int onBody(const char* d, size_t s) override
  {
    std::string str;
    str.assign(d, s);
    LOG(INFO) << "on body: " << str;
    return 0;
  }

};

TEST(http_client, normal)
{
  http::HttpClient client;
  client.get<FakeHttpHandler>("http://www.baidu.com", 1);
  client.get<FakeHttpHandler>("https://www.baidu.com", 1);
}
