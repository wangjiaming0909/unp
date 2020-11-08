#include "http/HttpClient.h"
#include "net/inet_addr.h"

#include <memory>
#include <netdb.h>

namespace http
{

HttpHandler::HttpHandler(reactor::Reactor& react, bool is_ssl)
  : sock_connection_handler(react, is_ssl)
{
}


HttpClient::HttpClient()
{
  client_ = std::make_shared<reactor::tcp_client>();
  url_parser_ = std::make_shared<URLParser>();
}
HttpClient::~HttpClient()
{

}

net::inet_addr HttpClient::retrive_addr()
{
  net::inet_addr ret;
  addrinfo* addrs = nullptr;

  char host[64] = {};
  memcpy(host, url_parser_->host().cbegin(), url_parser_->host().size());

  ::getaddrinfo(host, nullptr, nullptr, &addrs);

  if(addrs == nullptr)
  {
    LOG(ERROR) << "didn't get addrinfo: " << strerror(errno);
  }

  auto *ap = &addrs[0];

  if(ap)
  {
    sockaddr_in* inaddr = (sockaddr_in*)(ap->ai_addr);
    ret = *inaddr;
    // ap = ap->ai_next;
  }

  freeaddrinfo(addrs);
  if(url_parser_->is_https())
    ret.set_port_number(443);
  else
    ret.set_port_number(80);

}


}
