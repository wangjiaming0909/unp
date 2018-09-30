/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig::ServerConfig(string configFileName){
	setConfigFullPath(configFileName);
	m_read_config_file_ok = readConfigFile();
    parseConfigFile();
}

ServerConfig::~ServerConfig(){
	if(m_options_str != NULL)
		delete m_options_str;
}

bool ServerConfig::parseConfigFile(){
	std::string err;
	json11::Json cfgJson = json11::Json::parse(m_options_str->as_std_string(), err);
	auto maps = cfgJson.object_items();
	for(auto aPair : maps){
		string second = aPair.second.is_string() ? aPair.second.dump() : ""; 
		m_options_map.insert(
			OptionValidator::validateAndReturn(std::make_pair(string(aPair.first), second)));
	}
	return true;
}

bool ServerConfig::readConfigFile(){
	std::ifstream ifs;
	ifs.open(m_configFilePath.as_std_string());
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
	m_options_str = new string(buffer);
	if(buffer != NULL)
		delete buffer;
	return true;
}

void ServerConfig::setConfigFullPath(string& configFileName){
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
    temp = temp + configFileName.as_std_string();
    m_configFilePath = temp;
}

string ServerConfig::operator[](const string& key){
	if(m_options_map.count(key) == 0){
		CONSOLE_LOG("No this option");
	}
	return m_options_map[key];
}

string ServerConfig::operator[](const string& key)const{
	return (*this)[key];
}

string ServerConfig::operator[](const char* key) const{
	return this->operator[](key);
}