/*************************************************************************
	> File Name: Server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 09:08:21 AM CST
 ************************************************************************/
#include "Server.h"

Server::Server(const Config* config)
    : m_config(config){
    initialize();
}
Server::~Server(){

}

bool Server::start(){
    return true;
}
bool Server::stop(){
    return true;
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


    return true;
}
