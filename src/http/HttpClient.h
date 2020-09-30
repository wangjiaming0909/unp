#pragma once
#include "reactor/connector.h"
#include "reactor/tcp_client.h"
#include "http/HttpMessage.h"

namespace http
{

class HttpClient
{
public:
  HttpClient();
  ~HttpClient();
  template <typename Handler>
  reactor::connector<Handler>* get(const HttpMessage& mes);
  template <typename Handler>
  reactor::connector<Handler>* post(const HttpMessage& mes);

private:
  reactor::tcp_client *client_ = nullptr;
};


template <typename Handler>
reactor::connector<Handler>* HttpClient::get(const HttpMessage& mes)
{
}
template <typename Handler>
reactor::connector<Handler>* HttpClient::post(const HttpMessage& mes)
{

}








}
