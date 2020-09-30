#pragma once
#include "reactor/connector.h"
#include "reactor/tcp_client.h"

namespace http
{

class HttpClient
{
public:
  HttpClient();
  ~HttpClient();
  template <typename Handler>
  reactor::connector<Handler>* get();
  template <typename Handler>
  reactor::connector<Handler>* post();

private:
  reactor::tcp_client *client_ = nullptr;
};


template <typename Handler>
reactor::connector<Handler>* HttpClient::get()
{
}
template <typename Handler>
reactor::connector<Handler>* HttpClient::post()
{

}








}
