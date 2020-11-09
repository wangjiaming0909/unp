#include "http/HttpClient.h"
#include "http/HttpMethod.h"
#include "http/HttpMessage.h"
#include "Http1xCodec.h"
#include "net/inet_addr.h"

#include <memory>
#include <netdb.h>

namespace http
{

HttpHandler::HttpHandler(reactor::Reactor& react, bool is_ssl)
  : sock_connection_handler(react, is_ssl)
{
  codec_ = new Http1xCodec(HttpDirection::DOWNSTREAM);
  codec_->setCallback(this);
  mess_ = new HttpMessage();
}

HttpHandler::~HttpHandler()
{
  delete mess_;
  delete codec_;
}

int HttpHandler::handle_input(int handle)
{
  if(sock_connection_handler::handle_input(handle) < 0) {
    LOG(WARNING) << "error when sock_connection_handler::handle_input";
    return -1;
  }

  if(input_buffer_.buffer_length() == 0) {
    LOG(INFO) << "didn't get any data...";
    return 0;
  }

  while(input_buffer_.buffer_length() > 0) {
    auto firstChain = input_buffer_.begin().chain();
    auto data = firstChain.get_start_buffer();
    auto chainLen = firstChain.size();
    if(data != nullptr && chainLen != 0) {
      std::string s{static_cast<char*>(data), chainLen};
      LOG(INFO) << "received len: " << s.size();
      //LOG(INFO) << "received: " << s;
    }
    string_piece::const_string_piece sp{static_cast<const char*>(data), chainLen};
    size_t bytesRead = codec_->onIngress(sp);
    if(codec_->hasError()) {
      LOG(WARNING) << "Http parser parse error";
      codec_->pause(1);
      return -1;
    }
    assert(bytesRead == chainLen);
    input_buffer_.drain(chainLen);
    // LOG(INFO) << "Consumed " << chainLen << " bytes...";
  }
  return 0;
}
void HttpHandler::set_request_path(string_piece::const_string_piece path)
{
  if (mess_) {
    if (path.size() == 0)
      mess_->setRequestPath("/");
    else
      mess_->setRequestPath(path);
  }
}
void HttpHandler::set_method(HTTPMethod m)
{
  if (mess_) {
    mess_->setRequestMethod(m);
  }
}

void HttpHandler::set_host(string_piece::const_string_piece host)
{
  if (mess_) {
    mess_->addHeader(http::HttpHeaderCode::HTTP_HEADER_HOST, host);
  }
}

int HttpHandler::open()
{
  return 0;
}

int HttpHandler::send_request()
{
  mess_->setHttpVersion(1, 1);
  mess_->addHeader(http::HttpHeaderCode::HTTP_HEADER_USER_AGENT, USERAGENT);
  mess_->addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT, ACCEPT);
  mess_->addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING, ACCEPTENCODING);
  mess_->addHeader(http::HttpHeaderCode::HTTP_HEADER_CONNECTION, CONNECTION);

  auto mess_str = mess_->buildRequestMessage();
  LOG(INFO) << *mess_str;
  auto s = write(mess_str->c_str(), mess_str->size(), true);
  if (s <= 0) return -1;

  return enable_reading();
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
  return ret;
}


}
