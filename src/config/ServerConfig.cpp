/*************************************************************************
	> File Name: Config.cpp
	> Author:
	> Mail:
	> Created Time: Fri 28 Sep 2018 02:20:23 PM CST
 ************************************************************************/
#include "ServerConfig.h"
using namespace config;

ServerConfig* ServerConfig::instance(const char* file)
{
  static ServerConfig* instance = new ServerConfig(file);
  return instance;
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
  using namespace json11;
  json11::Json cfgJson = json11::Json::parse(*m_options_str, err);
  m_options_map = cfgJson.object_items();
  return err.empty();
}

int ServerConfig::readConfigFile(){
  LOG(INFO) << "class ServerConfig: reading the config file";
  util::FileUtil fileUtil(*m_configFilePath);
  size_t size_of_buffer = 512;
  this->m_options_str = new string();
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

bool ServerConfig::get_string_option(const string& key, string* ret){
  auto it = m_options_map.find(key);
  if(it == m_options_map.end() || it->second.type() != OptionType::STRING){
    LOG(WARNING) << "No this option or type error filename:  " << key;
    return false;
  }
  *ret = it->second.string_value();
  return true;
}
bool ServerConfig::get_number_option(const string& key, int* ret)
{
  auto it = m_options_map.find(key);
  if(it == m_options_map.end() || it->second.type() != OptionType::NUMBER){
    LOG(WARNING) << "No this option or type error filename:  " << key;
    return false;
  }
  *ret = it->second.int_value();
  return true;
}
bool ServerConfig::get_bool_option(const string& key, bool* ret)
{
  auto it = m_options_map.find(key);
  if(it == m_options_map.end() || it->second.type() != OptionType::BOOL){
    LOG(WARNING) << "No this option or type error filename:  " << key;
    return false;
  }
  *ret = it->second.bool_value();
  return true;
}
