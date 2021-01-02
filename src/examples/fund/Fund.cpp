#include "Fund.h"
#include <boost/format.hpp>
#include "config/ServerConfig.h"
#include "util/easylogging++.h"
#include "db/mysql_connection.h"

namespace examples
{


//"920921","ZJAXHBLHPZHHC","中金安心回报灵活配置混合C","混合型","ZHONGJINANXINHUIBAOLINGHUOPEIZHIHUNHEC"
int Fund::decode_from(const char* data, size_t len, Fund& f)
{
  char d[len+1];
  memcpy(d, data, len);
  d[len] = '\0';
  char c = '"';
  auto start = std::find(d, d+len, c);
  auto end = std::find(start+1, d+len, c);
  *end = '\0';
  f.code = start+1;
  start = std::find(end+1, d+len, c);
  end = std::find(start+1, d+len, c);
  *end = '\0';
  f.name_abbr = start+1;
  start = std::find(end+1, d+len, c);
  end = std::find(start+1, d+len, c);
  *end = '\0';
  f.name_chinese_ = start+1;
  start = std::find(end+1, d+len, c);
  end = std::find(start+1, d+len, c);
  *end = '\0';
  f.type = start+1;
  start = std::find(end+1, d+len, c);
  end = std::find(start+1, d+len, c);
  *end = '\0';
  f.name_pinyin_ = start+1;
}

FundDaliyWorker::FundDaliyWorker()
{
  fetcher_.reset(new FundFetcher());
  config::ServerConfig::instance()->get_string_option("fund_table_name", &fund_table_name_);
  config::ServerConfig::instance()->get_string_option("fund_company_table_name", &fund_company_table_name_);
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
  sql_conn_->execute("set names utf8");
}
void FundDaliyWorker::start(bool fetch_now)
{
  init_for_first_run();
  fetcher_->fetch_all_funds_info(all_funds_body_);
  decode_all_funds();
  replace_all_funds();
}

int FundDaliyWorker::replace_all_funds()
{
  const char* sql = "REPLACE INTO funds values('%s','%s','%s','%s','%s')";
  for (auto f : all_funds_map_) {
    auto formater = boost::format(sql)
      % f.second.code
      % f.second.name_abbr
      % f.second.name_chinese_
      % f.second.type
      % f.second.name_pinyin_;
    sql_conn_->execute(formater.str().c_str());
  }
}

int FundDaliyWorker::decode_all_funds()
{
  auto start = std::find(all_funds_body_.begin(), all_funds_body_.end(), '[');
  auto fund_start = std::find(start+1, all_funds_body_.end(), '[');
  while(fund_start != all_funds_body_.end()) {
    auto fund_end = std::find(fund_start, all_funds_body_.end(), ']');
    Fund f;
    Fund::decode_from((fund_start+1).base(), fund_end - fund_start - 1, f);
    auto ret = all_funds_map_.insert(std::make_pair(f.code, std::move(f)));
    if (!ret.second) {
      LOG(WARNING) << "decoded fund: " << f.code << " name: " << f.name_chinese_ << " failed";
    }
    fund_start = std::find(fund_end, all_funds_body_.end(), '[');
  }
  return 0;
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
  const char* sql_format = "CREATE TABLE IF NOT EXISTS %s %s";
  auto formater = boost::format(sql_format) % fund_table_name_
    % "(code varchar(16), name_abbr varchar(128), name varchar(256), type varchar(64), name_pinyin varchar(128),"
    " primary key(code))";
  if (sql_conn_->execute(formater.str().c_str())) {
    return -1;
  }
  formater = boost::format(sql_format) % fund_company_table_name_
    % "(code varchar(16), name varchar(256), primary key(name))";
  if (sql_conn_->execute(formater.str().c_str())) {
    return -1;
  }
  return 0;
}

int FundDaliyWorker::init_funds_table()
{
}

FundFetcher::FundFetcher()
{
  client_.reset(new http::HttpClient());
}

int FundFetcher::fetch_all_fund_companies()
{

}
int FundFetcher::fetch_all_funds_info(string& all_funds_body)
{
  string all_fund_url;
  config::ServerConfig::instance()->get_string_option("all_fund_url", &all_fund_url);
  if (client_->get<HttpBodyHandler>(all_fund_url.c_str(), all_funds_body)) {
    LOG(ERROR) << "fetch all funds error: ";
    return -1;
  }
  return 0;
}

int FundFetcher::fetch_fund_data(int code)
{

}
int FundFetcher::fetch_fund_data(std::vector<int>& codes)
{

}
int FundFetcher::fetch_fund_realtime_data(int code)
{

}

int HttpBodyHandler::onMessageComplete()
{
  should_close_ = true;
  return 0;
}

int HttpBodyHandler::onBody(const char* d, size_t len)
{
  LOG(DEBUG) << "got " << len << " bytes: " << d;
  body_.append(d, len);
  return 0;
}

}
