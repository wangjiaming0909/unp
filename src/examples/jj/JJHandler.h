#pragma once
#include "http/Http1xCodec.h"
#include "reactor/sock_connection_handler.h"
#include "http/HttpCodec.h"
#include "Fund.h"

namespace examples
{
using namespace http;
class JJHandler : public reactor::sock_connection_handler, HttpCodec::Callback
{
public:
  JJHandler(reactor::Reactor& react, bool isSSL = false);
  ~JJHandler();
  void fetch_all_fund_companies();
  void fetch_all_funds_info();
  void fetch_fund_data(int code);
  void fetch_fund_realtime_data(int code);
private:
  std::map<int, Fund> funds_;
};
}
