
#ifndef _SERVER_STATUS_H_
#define _SERVER_STATUS_H_

enum class SERVER_STATUS{
    Initialized,
    Binded,
    Started,
    Closed,
    Waiting_for_accept
};

#endif // _SERVER_STATUS_H_
