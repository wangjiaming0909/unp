#ifndef _UNP_REACTOR_SERVER_H_
#define _UNP_REACTOR_SERVER_H_
#include "config/ServerConfig.h"
#include "util/XString.h"
#include <vector>
#include "server_status.h"

namespace unp{

using namespace config;

class reactor_server{
public:
    reactor_server(ServerConfig& config);
    ~reactor_server();

    int start();
    int stop();
private:
    void initialize();
    void bind();
private:
    // string                  		        m_ipAddress;//
    // std::vector<std::shared_ptr<Client>>    m_clients;
    // size_t                  		        m_numOfClients = 0;
    ServerConfig&                           config_;
    SERVER_STATUS                           server_status_;

};
}
#endif // _UNP_REACTOR_SERVER_H_