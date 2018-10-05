/*************************************************************************
	> File Name: Config.h
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:19:32 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H
#include "ServerConfig.h"
#include <netinet/in.h>// for uint16_t
#include <string.h>
#include <map>
#include <iostream>
#include <cstdlib>
#include "json11.hpp"
#include <unistd.h>
#include <exception>
#include <utility>
#include "optionValidator.h"
#include "../util/XString.h"
#include <errno.h>
#include "../util/FileUtil.h"
#include "configoption.h"

#define CONSOLE_LOG(message) std::cout << message << std::endl;
namespace config{
using namespace util;

#define DEFAULT_CONFIG_JSON_FILE_NAME "server_conf.json"

class ServerConfig{
public:
	ServerConfig();
	ServerConfig(const string& configFileName);
    ~ServerConfig();
public:
//    string operator[](const char*) const;
    string operator[](const string& key);
//    string operator[](const string& key)const;
private:
	bool parseConfigFile();
    int readConfigFile();
    void setConfigFullPath(const string& configFileName);

private:
	// std::map<ConfigOption, ConfigOption> m_options_map2;
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
