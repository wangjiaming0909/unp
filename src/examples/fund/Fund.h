#pragma once
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mysql/mysql.h>


namespace examples
{

struct Fund
{
  int code;

  static std::unordered_map<int, Fund*> all_funds;
};



struct FundFetcher
{
  int fetch_all_fund_companies();
  int fetch_all_funds_info();

  int fetch_fund_data(int code);
  int fetch_fund_realtime_data(int code);

  std::vector<int> funds_to_fetch;
  std::unordered_map<int, Fund*> funds_fetched;
};

//FundDaliyWorker is a worker to fetch fund data daily
class FundDaliyWorker
{
public:
  FundDaliyWorker();
  void start(bool fetch_now = false);
  void stop();
  void set_time_point_of_day(unsigned int hour, unsigned int miniute, unsigned int second);

protected:
  int init_for_first_run();
  int init_tables();
private:
  unsigned int fetch_hour_ = 0;
  unsigned int fetch_miniute_ = 0;
  unsigned int fetch_second_ = 0;
  std::shared_ptr<FundFetcher> fetcher_;

  MYSQL* mysql;
};

}
