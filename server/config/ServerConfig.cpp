/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig::ServerConfig(std::string configFileName){
	setConfigFullPath(configFileName);
	m_read_config_file_ok = readConfigFile();
    parseConfigFile();
}

ServerConfig::~ServerConfig(){
	if(m_options_str != NULL)
		delete m_options_str;
}

bool ServerConfig::parseConfigFile(){
	using namespace std;
	string err;
	json11::Json cfgJson = json11::Json::parse(m_options_str->c_str(), err);
	auto maps = cfgJson.object_items();
	for(auto aPair : maps){
		string second = aPair.second.is_string() ? aPair.second.dump() : ""; 
		m_options_map.insert(
			OptionValidator::validateAndReturn(make_pair(aPair.first, second)));
	}
	return true;
}

bool ServerConfig::readConfigFile(){
	std::ifstream ifs;
	ifs.open(m_configFilePath);
	size_t size_of_buffer = 512;
	char* buffer = new char[size_of_buffer];
	memset(buffer, 0, size_of_buffer);
	ifs.read(buffer, size_of_buffer);
    if(ifs.eof()){
        ifs.close();
    }else if (!ifs){ //read error
        CONSOLE_LOG( " errno: " << strerror(errno));
		return false;
    }
	//CONSOLE_LOG(buffer);
	m_options_str = new std::string(buffer);
	if(buffer != NULL)
		delete buffer;
	return true;
}

void ServerConfig::setConfigFullPath(std::string& configFileName){
    char *cwd;
	try{
		cwd = get_current_dir_name();
	}catch(std::exception e){
		std::cout << strerror(errno) << std::endl;
		throw;
	}
	if(long(strlen(cwd) + configFileName.size()) > pathconf(cwd, _PC_NAME_MAX)){
		std::cout << "error file name too long" << std::endl;
		throw;
	}
	char dash = '/';
	cwd = strncat(cwd, &dash, 1);
    std::string temp = cwd;
    temp = temp + configFileName;
    m_configFilePath = temp;
}

std::string ServerConfig::operator[](const std::string& key){
	if(m_options_map.count(key) == 0){
		CONSOLE_LOG("No this option");
	}
	return m_options_map[key];
}

std::string ServerConfig::operator[](const std::string& key)const{
	return (*this)[key];
}

std::string ServerConfig::operator[](const char* key){
	std::string key_str = key;
	return this->operator[](key);
}