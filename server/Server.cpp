/*************************************************************************
	> File Name: Server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 09:08:21 AM CST
 ************************************************************************/
#include "Server.h"
using namespace server;

Server::Server(ServerConfig* config)
    : m_config(config){
    if(m_config == nullptr){
        throw "config_ptr is nullptr";
    }
    initialize();
    bind();
}

Server::~Server(){
    close(m_connfd);
    close(m_listenfd);
}

void Server::start(){
    string backlog = (*m_config)["backlog"];
    if(backlog == "")
        backlog = "5";
    int backlog_int = ::atoi(backlog.ptr());
    ::listen(m_listenfd, backlog_int);
    started = true;
}

void Server::stop(){

}

void Server::initialize(){
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listenfd == -1){
        CONSOLE_LOG("error socket ...");
        CONSOLE_LOG(strerror(errno));
        return;
    }
    bzero(&m_serverAddr, sizeof(m_serverAddr));   
    m_serverAddr.sin_family = AF_INET;
    string portStr = (*m_config)["port"];
    if(portStr == "")
        CONSOLE_LOG("config file error, no PORT specified");
    int port_int = atoi(portStr.ptr());
    m_serverAddr.sin_port = htons(uint16_t(port_int));
    initialized = true;
}

void Server::bind(){
    int ret = ::bind(
        m_listenfd,
        (sockaddr*)&m_serverAddr,
        sizeof(m_serverAddr));
    if(ret == -1){
        CONSOLE_LOG("error bind");
        CONSOLE_LOG(strerror(errno));
        return;
    }
    binded = true;
}
