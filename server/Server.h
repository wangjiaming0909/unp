/*************************************************************************
	> File Name: Server.h
	> Author: Jiaming WANG
	> Mail: 604227650@qq.com
	> Created Time: Fri 28 Sep 2018 08:54:56 AM CST
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H
#include <string>

class Server{
public:
    Server();
    ~Server();

public:
    void initialize();
    bool start();
    bool stop();

private:
    std::string m_ipAddress;
    std::string m_port;
};
#endif
