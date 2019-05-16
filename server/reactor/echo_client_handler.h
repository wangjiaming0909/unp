#ifndef _ECHO_CLIENT_HANDLER_H_
#define _ECHO_CLIENT_HANDLER_H_
#include "server/reactor/echo_connection_handler.h"
#include "server/util/easylogging++.h"

namespace reactor
{

class echo_client_handler : public echo_connection_handler
{
private:
    /* data */
public:
    echo_client_handler(Reactor& reactor) : echo_connection_handler(reactor){}
    ~echo_client_handler();
    virtual int open()
    {
        this->enable_reading();
        const char *data = "GET / HTTP/1.1\r\n\
Host: 192.168.0.2:8080\r\n\
Connection: keep-alive\r\n\
Cache-Control: max-age=0\r\n\
Upgrade-Insecure-Requests: 1\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36 OPR/58.0.3135.132\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7";
       int ret = this->write(data, ::strlen(data));
       if(ret != strlen(data))
       {
           LOG(WARNING) << "write error";
           return -1;
       }
       return 0;
    }
};

echo_client_handler::~echo_client_handler()
{
}

}//namespace reactor

#endif // _ECHO_CLIENT_HANDLER_H_