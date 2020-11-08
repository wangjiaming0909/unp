#pragma once
#include "http/HttpCodec.h"
#include "http/http_parser/URLParser.h"
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

class URLParser;

class HttpHandler : public reactor::sock_connection_handler, public http::HttpCodec::Callback
{
public:
  HttpHandler(reactor::Reactor& react, bool is_ssl);
  virtual ~HttpHandler() {}
protected:
  virtual int on_connected() = 0;
  virtual int on_closed() = 0;
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
    url_parser_->init(url);
    if (!url_parser_->valid()) {
      LOG(ERROR) << "url parse error";
      return -1;
    }
    bool is_ssl = url_parser_->is_https();
    auto* con = client_->addConnection<Connector<Handler>>(is_ssl, std::forward<Args>(args)...);
    auto addr = retrive_addr();
    using namespace std::chrono_literals;
    auto* connection = con->connect(addr, 1s);
    if (!connection) {
      LOG(ERROR) << "connect to: " << addr.get_address_string() << " error: " << strerror(errno);
      return -1;
    }
    if (connection->on_connected() != 0) {
      LOG(ERROR) << "handler on connected return not 0";
      client_->closeConnection(*con, 1s);
      return -1;
    }
    return client_->start();
  }
  template <typename Handler>
  int get(const char* url, std::shared_ptr<Handler> handler)
  {

  }

public:

private:
  net::inet_addr retrive_addr();

private:
  std::shared_ptr<reactor::tcp_client> client_ = nullptr;
  std::shared_ptr<URLParser> url_parser_ = nullptr;
};

}
