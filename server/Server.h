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
#include "Config.h"
#include <netinet/in.h>// for sockaddr_in
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <errno.h>

class Server{
public:
    Server(const Config* config);
    ~Server();

public:
    bool initialize();
    bool start();
    bool stop();

private:
    std::string             m_ipAddress;//
    std::vector<Client*>    m_clients;
    size_t                  m_numOfClients = 0;
    Config const            *m_config;
    int                     m_listenfd = 0;
    int                     connfd = 0;

//address
public:
    struct sockaddr_in      m_childAddr;
    struct sockaddr_in      m_serverAddr;
    std::string             m_port;//listen port
};
#endif
