/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
ServerConfig::ServerConfig(){

}
ServerConfig::ServerConfig(std::string configFilePath)
	: m_configFilePath(configFilePath){
	
}

ServerConfig::~ServerConfig(){

}

uint16_t ServerConfig::getPort(){

	return uint16_t(9999);
}

bool ServerConfig::parseConfigFile(){
	return true;
}