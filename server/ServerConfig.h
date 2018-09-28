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

class ServerConfig{
public:
    ServerConfig();
	ServerConfig(std::string configFilePath);
    ~ServerConfig();
public:
	uint16_t getPort();

private:
	bool parseConfigFile();

private:
	std::string 		m_configFilePath;
};

#endif
