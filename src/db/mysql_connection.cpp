#include "mysql_connection.h"
#include <mysql/mysql.h>
#include "util/easylogging++.h"
#include "config/ServerConfig.h"

namespace db
{


MYSQLConnection::MYSQLConnection()
  : SQLConenction()
{
}
MYSQLConnection::MYSQLConnection(
    const char* host,
    const char* user,
    const char* password,
    unsigned int port,
    const char* db)
  : SQLConenction(host, user, password, port, db)
{
  mysql_ = mysql_init(mysql_);
  if (!inited() || !mysql_real_connect(mysql_, host, user, password, db, port, 0, 0)) {
    LOG(ERROR) << "connect to " << host << "  error cause: " << mysql_error(mysql_);
  }
}

MYSQLConnection::~MYSQLConnection()
{
  if (inited()) {
    mysql_close(mysql_);
  }
}

int MYSQLConnection::execute(const char* sql)
{
  //LOG(INFO) << "executing sql: " << sql;
  TRY_EXECUTE_SQL(sql);
  if (mysql_field_count(mysql_)) {
    LOG(ERROR) << "execute sql: " << sql << " shouldn't have results";
    return -1;
  }
  return 0;
}

int MYSQLConnection::query_one_value(const char* sql, string& t)
{
  TRY_EXECUTE_SQL(sql);
  //auto field_count = mysql_field_count(mysql_);
}
int MYSQLConnection::query_one_row(const char* sql, vector<string>& t)
{}
int MYSQLConnection::query(const char* sql, vector<vector<string>>& data)
{}
}
