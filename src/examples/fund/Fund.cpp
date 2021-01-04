#include "Fund.h"
#include <algorithm>
#include <boost/format.hpp>
#include "config/ServerConfig.h"
#include "util/easylogging++.h"
#include "db/mysql_connection.h"

namespace examples
{
using namespace std;


int FundInfo::decode_from_string(const std::string& fund_info_str, FundInfo& fund)
{
  const char* fund_sourceRate = "fund_sourceRate";
  const char* fund_Rate = "fund_Rate";
  const char* fund_minsg = "fund_minsg";
  const char* stockCodes = "stockCodes";
  const char* Data_netWorthTrend = "Data_netWorthTrend";
  auto t_end = fund_info_str.end();
  auto it = std::search(fund_info_str.begin(), t_end, fund_sourceRate, fund_sourceRate + strlen(fund_sourceRate) - 1);
  if (it != t_end) {
    it = std::find(it, t_end, '"');
    auto end = std::find(it+1, t_end, '"');
    string s = std::string(it+1, end);
    fund.original_rate = atof(s.c_str());

    it = std::search(end, t_end, fund_Rate, fund_Rate + strlen(fund_Rate) - 1);
    it = std::find(it, t_end, '"');
    end = std::find(it+1, t_end, '"');
    s = std::string(it+1, end);
    fund.current_rate = atof(s.c_str());

    it = std::search(end, t_end, fund_minsg, fund_minsg + strlen(fund_minsg) - 1);
    it = std::find(it, t_end, '"');
    end = std::find(it + 1, t_end, '"');
    s = std::string(it+1, end);
    fund.minimal_purchase = atof(s.c_str());

    it = std::search(end, t_end, stockCodes, stockCodes + strlen(stockCodes) - 1);
    decode_stocks(&*it, &*t_end, fund);

    it = std::search(it, t_end, Data_netWorthTrend, Data_netWorthTrend + strlen(Data_netWorthTrend) - 1);
    decode_trend(&*it, &*t_end, fund);
  }
}

int FundInfo::decode_trend(const char* start, const char* end, FundInfo& fund)
{
  auto it = std::find(start, )
}

int FundInfo::decode_stocks(const char* start, const char* end, FundInfo& fund)
{
  char c = '"';
  auto it = std::find(start, end, c);
  auto it2 = std::find(start, end, ']');
  while(it != end && it < it2) {
    auto stock_code_end = std::find(it+1, it2, c);
    fund.stocks.emplace_back(it+1, stock_code_end);
    it = std::find(stock_code_end+1, it2, c);
  }
  return 0;
}

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
  if (fetch_all_fund_names) {
    replace_all_funds();
  }
  if (fetch_all_company) {
    fetcher_->fetch_all_fund_companies();
  }

  if (fetch_fund_info) {
    for (auto f : all_funds_map_) {
      string fund_info;
      FundInfo info;
      info.code = f.first;
      fetcher_->fetch_fund_data(info.code, fund_info);
      decode_fund_info(fund_info, info);
      replace_fund_info(info);
    }
  }
}

int FundDaliyWorker::decode_fund_info(const std::string& fund_info_str, FundInfo& fund)
{
  if (!FundInfo::decode_from_string(fund_info_str, fund)) {
    LOG(ERROR) << "decode fund info error";
    return -1;
  }
  return 0;
}
int FundDaliyWorker::replace_fund_info(FundInfo& f)
{
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

int FundFetcher::fetch_fund_data(const string& code, string& fund_body)
{
  string fund_info_url_prefix, fund_info_url_postfix;
  config::ServerConfig::instance()->get_string_option("fund_info_url_prefix", &fund_info_url_prefix);
  config::ServerConfig::instance()->get_string_option("fund_info_url_postfix", &fund_info_url_postfix);
  string url = fund_info_url_prefix + code + fund_info_url_postfix;
  if (client_->get<HttpBodyHandler>(url.c_str(), fund_body)) {
    LOG(ERROR) << "fetch fund info for: " << code << " error";
    return -1;
  }
  return 0;
}

int FundFetcher::fetch_fund_data(std::vector<string>& codes)
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
