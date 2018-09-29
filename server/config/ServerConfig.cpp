/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig::ServerConfig(std::string configFileName){
	setConfigFullPath(configFileName.c_str());
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
	std::ifstream ifs;
	ifs.open(*m_configFilePath);
	size_t size_of_buffer = 512;
	char* buffer = new char[size_of_buffer];
	memset(buffer, 0, size_of_buffer);
	ifs.read(buffer, size_of_buffer);
    if(ifs.eof()){
        //read over
        ifs.close();
    }else if (!ifs){ //read error
        CONSOLE_LOG( " errno: " << strerror(errno));
		return false;
    }
	CONSOLE_LOG(buffer);
	m_options_str = new std::string(buffer);
	if(buffer != NULL)
		delete buffer;
	return true;
}

void ServerConfig::setConfigFullPath(const char* configFileName){
    char *cwd = new char[_PC_NAME_MAX];
	try{
		cwd = get_current_dir_name();
	}catch(std::exception e){
		std::cout << strerror(errno) << std::endl;
		throw;
	}
	if(long(strlen(cwd) + strlen(configFileName)) > pathconf(cwd, _PC_NAME_MAX)){
		std::cout << "error file name too long" << std::endl;
		throw;
	}
	char dash = '/';
	cwd = strncat(cwd, &dash, 1);
	cwd = strcat(cwd, configFileName);
	m_configFilePath = new std::string(cwd);
}
