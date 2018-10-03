/*************************************************************************
	> File Name: Config.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig::ServerConfig(){
	ServerConfig(DEFAULT_CONFIG_JSON_FILE_NAME);
}

ServerConfig::ServerConfig(const string& configFileName){
	setConfigFullPath(configFileName);
    m_read_config_file_ok = !readConfigFile();
	if(!m_read_config_file_ok)
		exit(-1);
    parseConfigFile();
}

ServerConfig::~ServerConfig(){
    if(m_options_str != nullptr)
		delete m_options_str;
//    for(auto &option : m_options_map) {
//        if(option.first != nullptr){
//            delete option.first;
//        }
//        if(option.second != nullptr){
//            delete option.second;
//            option.second = nullptr;
//        }
//    }
}

bool ServerConfig::parseConfigFile(){
	std::string err;
	json11::Json cfgJson = json11::Json::parse(m_options_str->as_std_string(), err);
	auto maps = cfgJson.object_items();
//    string *first = new string();
//    string *second = new string();
    string first, second;
	for(auto aPair : maps){
        first.clear(); second.clear();
        auto s = aPair.second.is_string() ? aPair.second.dump() : "";
        first.append(aPair.first);//memcopy
        second.append(s);//memcopy
        // m_options_map.insert(std::pair<string, string>(first, second));
//       m_options_map[first] = second;
//		m_options_map.insert(
//            OptionValidator::validateAndReturn(new std::make_pair(first, second)));
	}
	return true;
}

int ServerConfig::readConfigFile(){
//	std::ifstream ifs;
//	ifs.open(m_configFilePath.as_std_string());
    FileUtil fileUtil(m_configFilePath);
    size_t size_of_buffer = 512;
    this->m_options_str = new string(size_of_buffer);
    int err = fileUtil.readToString(static_cast<int>(size_of_buffer), m_options_str);
    return err;
//    fileUtil.readToString(
//	char* buffer = new char[size_of_buffer];
//	memset(buffer, 0, size_of_buffer);
//	ifs.read(buffer, size_of_buffer);
//    fileUtil.readIn(&size_of_buffer);
//    buffer = fileUtil.buffer();
//    if(ifs.eof()){
//        ifs.close();
//    }else if (!ifs){ //read error
//        CONSOLE_LOG( " errno: " << strerror(errno));
//		return false;
//    }
	//CONSOLE_LOG(buffer);
//    m_options_str = new string(fileUtil.buffer());
//    if(buffer != nullptr)
//        delete[] buffer;
}

void ServerConfig::setConfigFullPath(const string& configFileName){
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
    m_configFilePath = temp;
    m_configFilePath.append(configFileName);
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
