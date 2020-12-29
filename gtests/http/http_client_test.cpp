#include <gtest/gtest.h>
#include "http/HttpClient.h"
#include "http/Http1xCodec.h"
#include "reactor/reactor.h"
#include "util/easylogging++.h"
#include "config/ServerConfig.h"


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

  virtual int onHeaderField(const char* header, size_t)
  {
    LOG(DEBUG) << "header field: " << header;
    return 0;
  }
  virtual int onHeaderValue(const char* value, size_t)
  {
    LOG(DEBUG) << "header value: " << value;
    return 0;
  }
  virtual int onBody(const char* d, size_t s) override
  {
    LOG(INFO) << "on body: " << d;
    return 0;
  }
  virtual int onMessageComplete() override
  {
    LOG(INFO) << "on message completed";
    should_close_ = true;
    return 0;
  }

};

TEST(http_client, normal)
{
  http::HttpClient client;
  //client.get<FakeHttpHandler>("http://www.baidu.com", 1);
  //client.get<FakeHttpHandler>("https://www.baidu.com", 1);
  //client.get<FakeHttpHandler>("https://www.3dmgame.com/", 1);
  std::string url;
  config::ServerConfig::instance()->get_string_option("all_fund_url", &url);
  LOG(DEBUG) << url;
  client.get<FakeHttpHandler>(url.c_str(), 1);

  config::ServerConfig::instance()->get_string_option("all_fund_company_url", &url);
  LOG(DEBUG) << url;
  client.get<FakeHttpHandler>(url.c_str(), 1);
}
