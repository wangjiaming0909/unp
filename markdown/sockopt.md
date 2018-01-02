## 套接字选项
### 两个函数
```c++
#include <sys/socket.h>
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
//成功返回0, 错误返回-1
```
### 1.`SOL_SOCKET`, 通用套接字选项, 即与协议无关的选项, 对应两个函数中的level参数
> `SO_BROADCAST` 开启或者禁止进程发送广播消息的能力 </br>
> `SO_DEBUG` 由`TCP`支持, 开启本选项, 内核将为`TCP`在该套接字发送和接受的所有分组保留详细的跟踪信息</br>
> `SO_DONTROUTE` 外出分组将绕过底层协议的正常路由机制</br>
>> 路由守护进程`routed`和`gated`经常使用本选项来绕过路由表(路由表不正确的情况下), 以强制将分组从特定接口发出</br>

> `SO_ERROR` 当当前套接字发生错误时, 可以通过此套接字选项获取错误</br>
> `SO_KEEPALIVE` </br>
> `SO_LINGER` 本选项控制调用`close`函数之后, 若发送缓冲区中有数据, 该如何操作</br>
>> 调用`close`函数, 如果不设置 `SO_LINGER`选项, 则对端`TCP`是否接收到数据都不知道 </br>
>> 如果设置了`SO_LINGER`选项, 则如果close返回成功返回, 则说明对端`TCP`已经接收到数据了,但是对端进程是否读取到数据不可得知</br>

>> `close`, `l\_onoff` = 0, 不设置该选项, 该套接字将发送缓冲区中的数据发送出去, 接受缓冲区中的数据废弃</br>
>> 设置该选项, `l_onoff` = 1, `l_linger` = 0, 丢弃发送和接受缓冲区数据, 直接发送一个`RST`, , 没有正常的终止序列, 没有`TIME_WAIT`状态, 没有`2MSL`时间</br>

> `SO_OOBINLINE` 有关带外数据 </br>
> `SO_RCVBUF` and `SO_SNDBUF` </br>
> `SO_RCVLOWAT` and `SO_SNDLOWAT`</br>
### 2.`IPPROTO_IP`</br>
### 3.`IPPROTO_ICMPV6`</br>
### 4.`IPPROTO_IPV6`</br>
### 5.`IPPROTO_IP`</br>
### 6.`IPPROTO_TCP`</br>
### 7.`IPPROTO_STCP`</br>