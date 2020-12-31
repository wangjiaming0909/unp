#pragma once
#include <string>
#include <vector>
namespace db
{

using namespace std;

class SQLConenction
{
public:
  SQLConenction(){}
  SQLConenction(
      const char* host, const char* user, const char* password, unsigned int port, const char* db)
    : host_(host), user_(user), password_(password), port_(port), db_(db) {}
  virtual ~SQLConenction() {}

  virtual int execute(const char* sql) = 0;
  virtual int query_one_value(const char* sql, string& t) = 0;
  virtual int query_one_row(const char* sql, vector<string>& t) = 0;
  virtual int query(const char* sql, vector<vector<string>>& data) = 0;

protected:
  string host_;
  string user_;
  string password_;
  unsigned int port_ = 3306;
  string db_;
};

}
