/*************************************************************************
	> File Name: Server.h
	> Author: Jiaming WANG
	> Mail: 604227650@qq.com
	> Created Time: Fri 28 Sep 2018 08:54:56 AM CST
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H
#include <string>
#include <vector>
#include "Client.h"
#include "config/ServerConfig.h"
#include <netinet/in.h>// for sockaddr_in
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include "util/XString.h"
#include "server_status.h"

namespace server{

using namespace config;
using namespace client;
using namespace util;

class Server{
public:
    Server(ServerConfig* config);
    ~Server();

public:
    void start();
    void stop();

public:
    bool initialized = false;
    bool binded = false;
    bool started = false;
    SERVER_STATUS get_status() const;

private:
    void initialize();
    void bind();

private:
    string                  		m_ipAddress;//
    std::vector<Client*>            m_clients;
    size_t                  		m_numOfClients = 0;
    ServerConfig      		        *m_config;
    int                     		m_listenfd = 0;
    int                     		m_connfd = 0;
    SERVER_STATUS                   m_server_status;

private:
    struct sockaddr_in      	    m_childAddr;
    struct sockaddr_in      	    m_serverAddr;
    uint16_t                	    m_port;//listen port
};
}

#endif
