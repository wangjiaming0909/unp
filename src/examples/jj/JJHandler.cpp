#include "JJHandler.h"
#include "reactor/reactor.h"
#include "config/ServerConfig.h"


namespace examples
{
JJHandler::JJHandler(reactor::Reactor& react, bool isSSL)
  : sock_connection_handler(react, isSSL)
{
}

JJHandler::~JJHandler()
{
}

void JJHandler::fetch_all_fund_companies()
{
}
void JJHandler::fetch_all_funds_info()
{
}
void JJHandler::fetch_fund_data(int code)
{
}
void JJHandler::fetch_fund_realtime_data(int code)
{
}
}
