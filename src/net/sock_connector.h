#ifndef _UNP_SOCK_CONNECTOR_H_
#define _UNP_SOCK_CONNECTOR_H_

#include "sock_stream.h"
#include "inet_addr.h"
#include "util/easylogging++.h"

namespace net
{

class sock_connector
{
  public:
    using micro_seconds = std::chrono::microseconds;

    sock_connector() {}
    ~sock_connector() {}
    int connect(SockStream &new_stream,
                const inet_addr &remote_addr,
                const micro_seconds *timeout,
                int reuse_addr, int protocol)
    {
        //socket get the socket fd
        if (this->shared_open(new_stream,
                              remote_addr.get_type(),
                              protocol, reuse_addr) == -1)
        {
            return -1;
        }
        new_stream.setNonBolcking(); //non-blocking connect
        auto addr_str = remote_addr.get_address_string();
        auto port = remote_addr.get_port_number();
        LOG(INFO) << "trying to connect to: " << addr_str << ":" << port;
        int ret = ::connect(new_stream.getHandle(),
                            reinterpret_cast<sockaddr *>(remote_addr.get_sockaddr_in_ptr().get()),
                            remote_addr.get_size());
        ret = connect_error_handling(new_stream, ret, timeout); //timeout used to wait for the connect
        if(ret != 0) return -1;
        return new_stream.connect();
    }

protected:
    //using SockStream, so the type is sock_type::stream
    int shared_open(SockStream &new_stream, int family,
                    int protocol, int reuse_addr)
    {
        if (new_stream.getHandle() == INVALID_HANDLE)
        {
            if (new_stream.openSockFD(family, sock_type::stream, protocol, reuse_addr) == -1)
                return -1;
        }
        else
        {
            new_stream.close();
            if (new_stream.openSockFD(family, sock_type::stream, protocol, reuse_addr) == -1)
                return -1;
        }
        return 0;
    }

  private:
    int connect_error_handling(SockStream &new_stream, int ret, const micro_seconds *timeout)
    {
        if (ret < 0)
        {
            if (errno != EINPROGRESS)
                return -1; //other errors, check the errno yourself
        }
        else if (ret == 0)
        { //connect completed, when client and server are on the same host machine, this could happen
            LOG(INFO) << "connect succeed...";
            new_stream.restoreBlocking();
            return 0;
        }
        //errno == EINPROGRESS that's what we expected
        return complete(new_stream, timeout);
    }

    int complete(SockStream &new_stream, const micro_seconds *timeout)
    {
        //TODO what if timeout is nullptr
        auto timeout_milli_seconds = std::chrono::duration_cast<milliseconds>(*timeout);
        int h = unp::handle_timed_connect_using_select(
            new_stream.getHandle(),
            &timeout_milli_seconds);
        //timeout or poll error
        if (h == INVALID_HANDLE)
        {
            LOG(INFO) << "connect error: " << strerror(errno);
            new_stream.close(); //we need to close the fd
            return -1;
        }
        //poll success, connect success
        new_stream.restoreBlocking(); //you can read now
        return 0;
    }
};
} // namespace net

#endif // _UNP_SOCK_CONNECTOR_H_
