#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <algorithm>


void hide_pwd(std::string& conn_str)
{
  std::string tmp = conn_str;
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
  size_t start_pos = tmp.find("PWD");
  if(start_pos == std::string::npos) {
    return;
  }
  size_t semi_colon_pos = tmp.find(';', start_pos);
  if(semi_colon_pos == std::string::npos) {
    conn_str.erase(start_pos);
    return ;
  }
  size_t left_brace_pos = tmp.find('{', start_pos);
  if(left_brace_pos == std::string::npos || left_brace_pos > semi_colon_pos) {
    conn_str.erase(start_pos, semi_colon_pos - start_pos + 1);
    return;
  }
  //;PWD={***;***};
  size_t right_brace_pos = tmp.find('}', semi_colon_pos);
  if(right_brace_pos != std::string::npos) {
    size_t next_semi_colon_pos = tmp.find(';', right_brace_pos);
    if(next_semi_colon_pos == std::string::npos)
      conn_str.erase(start_pos);
    else 
      conn_str.erase(start_pos, next_semi_colon_pos - start_pos + 1);
    return;
  } else
  {
    conn_str.erase(start_pos, semi_colon_pos - start_pos + 1);
  }

}

TEST(remove_pwd, normal)
{
    std::string connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD=;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD =012asapwd=;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD =012asapwd=;ABC=10;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD ={012a;sapwd=};ABC=10;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;

    connStr = "DRIVER=MySQL ODBC 5.3;DATABASE=mysql;SERVER=127.0.0.1;PORT=13006;UID=root;PWD ={012a;sapwd=;ABC=10;";
    hide_pwd(connStr);
    std::cout << connStr << std::endl;
}
