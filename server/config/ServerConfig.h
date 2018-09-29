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
#include <string>
#include <string.h>
#include <map>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "json11.hpp"
#include <unistd.h>

namespace config{

#define DEFAULT_CONFIG_JSON_FILE_NAME "./server_conf.json"

class ServerConfig{
public:
	ServerConfig(std::string configFileName = DEFAULT_CONFIG_JSON_FILE_NAME);
    ~ServerConfig();
public:
	

private:
	bool parseConfigFile();
	bool readConfigFile();
	void getPwd();

private:
	std::map<std::string, std::string> 	m_options_map;
	std::string 						m_configFilePath;
	std::string*						m_options_str;

private:
	bool 								m_read_config_file_ok;
public:
	bool getReadConfigFileStatus(){return m_read_config_file_ok;}
};
}
#endif
