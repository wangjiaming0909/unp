#pragma once
#include "http/HttpCodec.h"
#include "http/http_parser/URLParser.h"
#include "http/HttpMethod.h"
#include "reactor/connector.h"
#include "reactor/sock_connection_handler.h"
#include "reactor/tcp_client.h"
#include "net/inet_addr.h"
#include "util/easylogging++.h"

#include <memory>
#include <exception>

namespace reactor
{class tcp_client;
}

namespace http
{

class HttpMessage;
class Http1xCodec;

class URLParser;

static string_piece::const_string_piece USERAGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:83.0) Gecko/20100101 Firefox/83.0";
//static string_piece::const_string_piece ACCEPT = "*/*";
static string_piece::const_string_piece ACCEPT = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
static string_piece::const_string_piece ACCEPTENCODING = "deflate";
//static string_piece::const_string_piece ACCEPTENCODING = "identity";
static string_piece::const_string_piece ACCEPTLANGUAGE = "en-US,zh-CN;q=0.8,zh;q=0.7,zh-TW;q=0.5,zh-HK;q=0.3,en;q=0.2";
static const std::string DEFAULT_FILE_NAME = "unp.unp";
static string_piece::const_string_piece CONNECTION = "keep-alive";
static string_piece::const_string_piece PRAGMA = "no-cache";
static string_piece::const_string_piece CACHE_CONTROL = "no-cache";
static string_piece::const_string_piece UPGRADE_INSECURE_REQUESTS = "1";

class HttpHandler : public reactor::sock_connection_handler, public http::HttpCodec::Callback
{
public:
  HttpHandler(reactor::Reactor& react, bool is_ssl);
  virtual ~HttpHandler();
  virtual int open() override;
  virtual int handle_input(int) override;
  void set_request_path(string_piece::const_string_piece path);
  void set_method(HTTPMethod m);
  void set_host(string_piece::const_string_piece host);
  int send_request();

  virtual int on_connected() {return 0;}
  virtual int on_closed() {return 0;}

protected:
  HttpMessage* mess_;
  Http1xCodec* codec_;
  bool should_close_ = false;
};

class HttpClient
{
public:
  template <typename Handler>
  using Connector = reactor::connector<Handler>;
  HttpClient();
  ~HttpClient();
  template <typename Handler, typename ...Args>
  int get(const char* url, Args&&... args)
  {
    //LOG(INFO) << "url: " << url;
    url_parser_->reset(url);
    if (!url_parser_->valid()) {
      LOG(ERROR) << "url parse error";
      return -1;
    }
    auto* con = client_->addConnection<Connector<Handler>>(url_parser_->is_https(), std::forward<Args>(args)...);
    auto addr = retrive_addr();
    using namespace std::chrono_literals;
    auto* connection = con->connect(addr, 1s);
    if (!connection) {
      LOG(ERROR) << "connect to: " << addr.get_address_string() << " error: " << strerror(errno);
      return -1;
    }
    connection->set_method(HTTPMethod::GET);
    connection->set_request_path(url_parser_->path());
    connection->set_host(url_parser_->host());
    connection->send_request();
    if (connection->on_connected() != 0) {
      LOG(ERROR) << "handler on connected return not 0";
      client_->closeConnection(*con, 1s);
      return -1;
    }
    auto ret = client_->start();
    client_->closeConnection(*con, 1s);
    if (ret == -2) {
      ret = 0;
    }
    return ret;
  }
private:
  net::inet_addr retrive_addr();

private:
  std::shared_ptr<reactor::tcp_client> client_ = nullptr;
  std::shared_ptr<URLParser> url_parser_ = nullptr;
};

}
