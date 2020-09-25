#pragma once
#include "http/Http1xCodec.h"
#include "reactor/sock_connection_handler.h"
#include "http/HttpCodec.h"

namespace examples
{
using namespace http;
class JJHandler : public reactor::sock_connection_handler, HttpCodec::Callback
{
public:
  JJHandler(reactor::Reactor& react, bool isSSL = false);
  ~JJHandler();
private:
};
}
