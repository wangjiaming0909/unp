#include "Fund.h"
#include <string>
#include <boost/format.hpp>
#include "config/ServerConfig.h"

namespace examples
{


FundDaliyWorker::FundDaliyWorker()
{
  fetcher_.reset(new FundFetcher());
  mysql_init(mysql);
}
void FundDaliyWorker::start(bool fetch_now)
{

}
void FundDaliyWorker::stop()
{

}

void FundDaliyWorker::set_time_point_of_day(unsigned int hour, unsigned int miniute, unsigned int second)
{
  this->fetch_hour_ = hour;
  this->fetch_miniute_ = miniute;
  this->fetch_second_ = second;
}

int FundDaliyWorker::init_for_first_run()
{
  init_tables();
}

int FundDaliyWorker::init_tables()
{
  string fund_table_name;
  config::ServerConfig::instance()->get_string_option("fund_table_name", &fund_table_name);
  const char* sql_format = "CREATE TABLE IF NOT EXISTS %s %s";
  auto formater = boost::format(sql_format) % fund_table_name % "(code int)";
  mysql_query(mysql, formater.str().c_str());

  //TODO other tables
}

int FundFetcher::fetch_all_fund_companies()
{

}
int FundFetcher::fetch_all_funds_info()
{}
int FundFetcher::fetch_fund_data(int code)
{}
int FundFetcher::fetch_fund_realtime_data(int code)
{}


}
