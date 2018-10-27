/*************************************************************************
	> File Name: Server.cpp
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 09:08:21 AM CST
 ************************************************************************/
#include "Server.h"
#include "util/easylogging++.h"
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
    LOG(INFO) << "trying to start the server...";
    // string backlog = (*m_config)["backlog"];
    // if(backlog == "")
    //     backlog = "5";
    // LOG(INFO) << "get the backlog.. ";
    // LOG(INFO) << backlog.as_std_string();
    // int backlog_int = ::atoi(backlog.ptr());
    // LOG(INFO) << "after converting to int...";
    // LOG(INFO) << backlog_int;
    // ::listen(m_listenfd, backlog_int);
    socket::listen(m_listenfd);
    started = true;
    m_server_status = SERVER_STATUS::Started;
    //accept
    LOG(INFO) << "on accepting.....";
    std::shared_ptr<Client> client_ptr{new Client()};
    m_clients.push_back(client_ptr);
    // m_connfd = ::accept(m_listenfd,
    //                     reinterpret_cast<sockaddr*>(&m_clients[0]->m_addr),
    //                     &(client_ptr->m_sock_len));
    m_connfd = socket::accept(m_listenfd, &client_ptr->m_addr); 
    if(m_connfd == -1){
        LOG(ERROR) << "connection file descriptor is -1...";
        exit(-1);
    }
    LOG(INFO) << "accepted a connection from: ";
    LOG(INFO) << socket::from_in_addr_to_string(client_ptr->m_addr.sin_addr).as_std_string();
    if(m_connfd >= 0)//! won't get here because it will stuck at accept
        m_server_status = SERVER_STATUS::Waiting_for_accept;
}

void Server::stop(){
    LOG(INFO) << "trying to stop the server... ";
    if(m_connfd != 0)
        close(m_connfd);
    if(m_listenfd != 0)
        close(m_listenfd);
    m_server_status = SERVER_STATUS::Closed;
}

SERVER_STATUS Server::get_status() const
{
    return m_server_status;
}

void Server::initialize(){
    m_listenfd = socket::create_socket_nonblock_or_die(AF_INET);
    bzero(&m_serverAddr, sizeof(m_serverAddr));   
    m_serverAddr.sin_family = AF_INET;
//    LOG(INFO) << "trying to read the port conf";
    string portStr = (*m_config)["port"];
    if(portStr == "")
        LOG(FATAL) << "config file error, no PORT specified";
    m_serverAddr.sin_port = htons(int(portStr));//TODO need transfer the format
    initialized = true;
    m_server_status = SERVER_STATUS::Initialized;
    LOG(INFO) << "class Server: server has been initialized...";
}

void Server::bind(){
    LOG(INFO) << "class Server: binding to address: ";
    LOG(INFO) << m_serverAddr.sin_port;
    // int ret = ::bind( m_listenfd, reinterpret_cast<sockaddr*>(&m_serverAddr), sizeof(m_serverAddr));
    socket::bind_or_close(m_listenfd, &m_serverAddr);
    binded = true;
    m_server_status = SERVER_STATUS::Binded;
}
