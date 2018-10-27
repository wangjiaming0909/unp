/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig::ServerConfig() 
	: ServerConfig(DEFAULT_CONFIG_JSON_FILE_NAME){
}

ServerConfig::ServerConfig(const string& configFileName){
	setConfigFullPath(configFileName);
    m_read_config_file_ok = !readConfigFile();
	if(!m_read_config_file_ok){
		LOG(ERROR) << ("fd is not ready");
	}
    parseConfigFile();
}

ServerConfig::~ServerConfig(){
    LOG(INFO) << "class ServerConfig: deconstructing...";
    if(m_options_str != nullptr){
        delete m_options_str;
	}
	if(m_configFilePath != nullptr){
		delete m_configFilePath;
	}
}

bool ServerConfig::parseConfigFile(){
    LOG(INFO) << "class ServerConfig: parsing the config file";
	std::string err;
	const std::string tmp_str = m_options_str->as_std_string();
	json11::Json cfgJson = json11::Json::parse(tmp_str, err);
    auto maps = cfgJson.object_items();
    string first, second;
	for(auto aPair : maps){
        first.clear(); second.clear();
        auto s = aPair.second.dump();
        first.append(aPair.first);//memcopy
        second.append(s);//memcopy
		m_options_map.insert(OptionValidator::validateAndReturn(
			std::pair<string, string>(first, second)));
	}
    return err.empty();
}

int ServerConfig::readConfigFile(){
    LOG(INFO) << "class ServerConfig: reading the config file";
    FileUtil fileUtil(*m_configFilePath);
    size_t size_of_buffer = 512;
    this->m_options_str = new string(size_of_buffer);
    int err = fileUtil.readToString(static_cast<int>(size_of_buffer), m_options_str);
    return err;
}

void ServerConfig::setConfigFullPath(const string& configFileName){
    LOG(INFO) << "class ServerConfig: seting the config file path";
    char *cwd = nullptr;
    cwd = get_current_dir_name();
	if(long(strlen(cwd) + configFileName.size()) > pathconf(cwd, _PC_NAME_MAX)){
		std::cout << "error file name too long" << std::endl;
		throw;
	}
	string dash = "/";
	string tmp = cwd;
	m_configFilePath = new util::string(cwd);
	m_configFilePath->append(dash);
	m_configFilePath->append(configFileName);
}

string ServerConfig::operator[](const string& key){
    if(m_options_map.count(key) == 0){
		std::string message = "No this option.. filename:  " + key.as_std_string();
		LOG(WARNING) << message;
		return "";
    }
    return m_options_map[key];
}

//string ServerConfig::operator[](const string& key)const{
//    return m_options_map[;
//    return (*this)[key];
//}

//string ServerConfig::operator[](const char* key) const{
//	return this->operator[](key);
//}
