#include <map>
#include <string>
#include "config/ServerConfig.h"
#include "main_helper.h"
#include "util/easylogging++.h"
using namespace std;

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP


static std::map<string, std::function<int ()>> funcs
{
};

int main(int argc, char** argv)
{
  server_scoped_helper s_h{argc, argv};
  auto* cf = config::ServerConfig::instance();
  string func;
  auto ret = cf->get_string_option("func", &func);
  if (!ret) {
    LOG(DEBUG) << "no func configured";
    return -1;
  }
  auto it = funcs.find(func);
  if (it != funcs.end()) {
    LOG(DEBUG) << "Running " << func;
    return it->second();
  } else
    LOG(DEBUG) << "no funcs to run";
  return 0;
}


