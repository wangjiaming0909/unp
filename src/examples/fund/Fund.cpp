#include "Fund.h"
#include <boost/format.hpp>
#include "config/ServerConfig.h"
#include "util/easylogging++.h"
#include "db/mysql_connection.h"

namespace examples
{


FundDaliyWorker::FundDaliyWorker()
{
  fetcher_.reset(new FundFetcher());
  config::ServerConfig::instance()->get_string_option("fund_table_name", &fund_table_name_);
  string host, user, passwd, db;
  int port;
  config::ServerConfig::instance()->get_string_option("mysql_server_host", &host);
  config::ServerConfig::instance()->get_string_option("mysql_server_user", &user);
  config::ServerConfig::instance()->get_string_option("mysql_server_password", &passwd);
  config::ServerConfig::instance()->get_string_option("mysql_server_db", &db);
  config::ServerConfig::instance()->get_number_option("mysql_server_port", &port);
  sql_conn_.reset(new db::MYSQLConnection(
        host.c_str(),
        user.c_str(),
        passwd.c_str(),
        static_cast<unsigned int>(port),
        db.c_str()));
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
  if (init_tables()) {
    return -1;
  }
  return 0;
}

int FundDaliyWorker::init_tables()
{
  init_funds_table();
}

int FundDaliyWorker::init_funds_table()
{
  const char* sql_format = "CREATE TABLE IF NOT EXISTS %s %s";
  auto formater = boost::format(sql_format) % fund_table_name_ % "(code int)";
  if (sql_conn_->execute(formater.str().c_str())) {
    return -1;
  }
}

int FundFetcher::fetch_all_fund_companies()
{

}
int FundFetcher::fetch_all_funds_info()
{}
int FundFetcher::fetch_fund_data(int code)
{

}
int FundFetcher::fetch_fund_data(std::vector<int>& codes)
{

}
int FundFetcher::fetch_fund_realtime_data(int code)
{}


}
