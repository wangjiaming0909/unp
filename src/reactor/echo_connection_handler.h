#ifndef ECHO_CONNECTION_HANDLER_H_
#define ECHO_CONNECTION_HANDLER_H_
#include "reactor/connection_handler.h"
#include "boost/asio/buffer.hpp"
#include "boost/beast/core/error.hpp"

namespace reactor
{

class echo_connection_handler : public connection_handler
{
public:
    echo_connection_handler(Reactor& reactor);
    virtual ~echo_connection_handler();
    virtual int handle_input(int handle) override;
private:
    // static int i;
};


struct Stream{
    connection_handler& handler_;
    Stream(connection_handler& handler) : handler_(handler){}

    void operator()(boost::beast::error_code&, const boost::asio::mutable_buffer&)
    {

    }
    std::size_t write_some(const boost::asio::mutable_buffer& buffer, boost::beast::error_code&)
    {
        auto data = static_cast<char*>(buffer.data());
        auto ret = handler_.write(data, buffer.size());
        return ret;
    }

    std::size_t write_some(const boost::asio::mutable_buffer& buffer)
    {
        auto data = static_cast<char*>(buffer.data());
        auto ret = handler_.write(data, buffer.size());
        return ret;
    }
};


class WriteLambda
{
    connection_handler& handler_;
public:
    explicit WriteLambda(connection_handler& handler) : handler_(handler) { }

    template<class ConstBufferSequence>
    void operator()(boost::beast::error_code& , ConstBufferSequence const& buffers)
    {
        auto begin = buffers.begin();
        auto end = buffers.end();

        for(auto it = begin; it != end; it++)
        {
            boost::asio::const_buffer b = *it;
            handler_.write(static_cast<const char*>(b.data()), b.size());
        }
    }
};



    
}
#endif