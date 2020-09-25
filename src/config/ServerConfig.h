/*************************************************************************
	> File Name: Config.h
	> Author:
	> Mail:
	> Created Time: Fri 28 Sep 2018 02:19:32 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H
#include <netinet/in.h>// for uint16_t
#include <errno.h>
#include <string.h>

#include <map>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <utility>

#include "json11.hpp"
#include "optionValidator.h"
#include "util/FileUtil.h"
#include "configoption.h"
#include "util/easylogging++.h"

namespace config{
using std::string;

#define DEFAULT_CONFIG_JSON_FILE_NAME "server_conf.json"

class ServerConfig{
public:
  ~ServerConfig();
  static ServerConfig* instance(const char*file = DEFAULT_CONFIG_JSON_FILE_NAME);
private:
  ServerConfig(const string& configFileName);
public:
  const char* operator[](const string& key);
private:
  bool parseConfigFile();
  int readConfigFile();
  void setConfigFullPath(const string& configFileName);

private:
  std::map<string, string>		 	m_options_map;
  string* 							m_configFilePath = nullptr;
  string*								m_options_str = nullptr;

private:
  bool 								m_read_config_file_ok;
public:
  bool getReadConfigFileStatus(){return m_read_config_file_ok;}
};
}
#endif
