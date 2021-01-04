#pragma once
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "db/sql_connection.h"
#include "http/HttpClient.h"


namespace examples
{

struct FundInfo
{
  std::string code;
  double original_rate;
  double current_rate;
  double minimal_purchase;
  std::vector<std::string> stocks;
  std::vector<double> growth;//index means day
  std::unordered_map<unsigned long, double> growth_map;
  std::string fund_manager;

  static int decode_from_string(const std::string& fund_info_str, FundInfo& fund);
  static int decode_stocks(const char* start, const char* end, FundInfo& fund);
  static int decode_trend(const char* start, const char* end, FundInfo& fund);
};


struct Fund
{
  Fund(){}
  Fund(const Fund&f) = default;
  Fund(Fund&& f)
  {
    code = std::move(f.code);
    name_abbr = std::move(f.name_abbr);
    name_chinese_ = std::move(f.name_chinese_);
    type = std::move(f.type);
    name_pinyin_ = std::move(f.name_pinyin_);
  }
  std::string code;
  std::string name_abbr;
  std::string name_chinese_;
  std::string type;
  std::string name_pinyin_;
  static int decode_from(const char* data, size_t len, Fund& f);
  static std::unordered_map<int, Fund*> all_funds;
  FundInfo info;
};

class HttpBodyHandler : public http::HttpHandler
{
public:
  HttpBodyHandler(reactor::Reactor& react, bool is_ssl, std::string& body)
    : HttpHandler(react, is_ssl), body_(body) {}
  virtual int onBody(const char* d, size_t len) override;
  virtual int onMessageComplete() override;

private:
  std::string& body_;
};

struct FundFetcher
{
  FundFetcher();
  int fetch_all_fund_companies();
  int fetch_all_funds_info(std::string& all_funds_body);

  int fetch_fund_data(const std::string& code, std::string& fund_body);
  int fetch_fund_data(std::vector<std::string>& codes);
  int fetch_fund_realtime_data(int code);

  std::unordered_map<int, Fund*> funds_fetched;

private:
  std::shared_ptr<http::HttpClient> client_;
};

//FundDaliyWorker is a worker to fetch fund data daily
class FundDaliyWorker
{
public:
  FundDaliyWorker();
  void start(bool fetch_now = false);
  void stop();
  void set_time_point_of_day(unsigned int hour, unsigned int miniute, unsigned int second);

public:
  int init_for_first_run();
  int init_tables();
  int init_funds_table();
  int init_fund_company_table();
  const std::string& get_all_funds_body() const {return all_funds_body_;}
  const Fund* get_fund(const std::string& code) const {
    auto it = all_funds_map_.find(code);
    if (it != all_funds_map_.end()) {
      return &it->second;
    }
    return nullptr;
  }

  bool fetch_all_fund_names = false;
  bool fetch_all_company = false;
  bool fetch_fund_info = true;

protected:
  int decode_all_funds();
  int replace_all_funds();
  int decode_fund_info(const std::string& fund_info_str, FundInfo& fund);
  int replace_fund_info(FundInfo& f);
private:
  unsigned int fetch_hour_ = 0;
  unsigned int fetch_miniute_ = 0;
  unsigned int fetch_second_ = 0;
  std::shared_ptr<FundFetcher> fetcher_;

  std::shared_ptr<db::SQLConenction> sql_conn_;
  std::string fund_table_name_;
  std::string fund_company_table_name_;
  std::string all_funds_body_;
  std::unordered_map<std::string, Fund> all_funds_map_;
};

}
