/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig::ServerConfig(std::string configFilePath)
	: m_configFilePath(configFilePath){
	m_read_config_file_ok = readConfigFile();
}

ServerConfig::~ServerConfig(){
	if(m_options_str != NULL)
		delete m_options_str;
}

bool ServerConfig::parseConfigFile(){
	json11::Json jsonParser();
	return true;
}

bool ServerConfig::readConfigFile(){
	std::ifstream ifs(m_configFilePath);
	size_t size_of_buffer = 512;
	char* buffer = new char[size_of_buffer];
	memset(buffer, 0, size_of_buffer);
	ifs.read(buffer, size_of_buffer);
	if(!ifs){//read error
		std::cout << "ifs read error" << std::endl;
		return false;
	}
	m_options_str = new std::string(buffer);
	if(buffer != NULL)
		delete buffer;
	return true;
}