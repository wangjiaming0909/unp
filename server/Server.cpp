/*************************************************************************
	> File Name: Server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 09:08:21 AM CST
 ************************************************************************/
#include "Server.h"
using namespace server;

Server::Server(const ServerConfig* config)
    : m_config(config){
    initialize();
    this->bind();
}


Server::~Server(){

}

void Server::start(){
    
}

void Server::stop(){

}

bool Server::initialize(){
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listenfd == -1){
        std::cout << "error socket ..." << std::endl;
        std::cout << strerror(errno) << std::endl;
        return false;    
    }
    bzero(&m_serverAddr, sizeof(m_serverAddr));   
    m_serverAddr.sin_family = AF_INET;
    std::string portStr = (*m_config)["port"];
    int port_int = atoi(portStr.c_str());
    m_serverAddr.sin_port = htons(uint16_t(port_int));
    return true;
}

void Server::bind(){
    int ret = ::bind(
        m_listenfd, 
        (sockaddr*)&m_serverAddr, 
        sizeof(m_serverAddr));
    if(ret == -1){
        CONSOLE_LOG("error bind");
        CONSOLE_LOG(strerror(errno));
        exit(-1);
    }
}
