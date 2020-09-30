#include "FundHandler.h"
#include "reactor/reactor.h"
#include "config/ServerConfig.h"


namespace examples
{
FundHandler::FundHandler(reactor::Reactor& react, bool isSSL)
  : sock_connection_handler(react, isSSL)
{
}

FundHandler::~FundHandler()
{
}

void FundHandler::fetch_all_fund_companies()
{
}
void FundHandler::fetch_all_funds_info()
{
}
void FundHandler::fetch_fund_data(int code)
{
}
void FundHandler::fetch_fund_realtime_data(int code)
{
}
}
