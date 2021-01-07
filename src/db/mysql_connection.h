#pragma once
#include "sql_connection.h"
#include <mysql/mysql.h>


namespace db
{
#define TRY_EXECUTE_SQL(sql) \
  do {\
    if (!inited() || mysql_real_query(mysql_, (sql), strlen((sql)))) {\
      LOG(ERROR) << "execute sql: " << (sql) << " error: " << mysql_error(mysql_);\
      return -1;\
    }\
  } while(0)

class MYSQLConnection : public SQLConenction
{
public:
  MYSQLConnection();
  MYSQLConnection(
      const char* host,
      const char* user,
      const char* password,
      unsigned int port,
      const char* db = "information_schema");
  ~MYSQLConnection();

  virtual int execute(const char* sql) override;
  virtual int query_one_value(const char* sql, string& t) override;
  virtual int query_one_row(const char* sql, vector<string>& t) override;
  virtual int query(const char* sql, vector<vector<string>>& data) override;

protected:
  bool inited() const { return mysql_; }
  int try_execute_sql(const char* sql);

protected:
  MYSQL* mysql_ = nullptr;
};
}
