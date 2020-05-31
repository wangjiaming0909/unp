#include "reactor/echo_connection_handler.h"
#include <iostream>
#include <chrono>
#include "util/unp_time.h"
#include "boost/beast/http.hpp"
#include "boost/beast/http/empty_body.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core.hpp"

using namespace reactor;

// int echo_connection_handler::i = 0;

echo_connection_handler::echo_connection_handler(Reactor& reactor) : sock_connection_handler(reactor)
{
    // util::string time = util::Time::now().toString();
    // std::cout << "constructing echo connectin handler " << i << std::endl;
    // i++;
}

echo_connection_handler::~echo_connection_handler()
{
    LOG(INFO) << "closing a connection handle: " << this->get_handle();
    // util::string time = util::Time::now().toString();
    // std::cout << "destructing echo connection handler "<< i << std::endl;
    // i--;
}

int echo_connection_handler::handle_input(int handle)
{
    LOG(INFO) << "handle: " << handle << " going to read...";
    if(input_buffer_.total_len() >= sock_connection_handler::BUFFER_HIGH_WATER_MARK)
    {
        return 0;
    }

    if(handle != stream_->get_handle() || handle == INVALID_HANDLE)
    {
        LOG(ERROR) << "Register error: handle error: " << handle;
        return -1;
    }

    int recv_buf_size = 4096;

    int ret = stream_->read(input_buffer_, recv_buf_size);
    if(ret < 0)
    {
        LOG(ERROR) << "Read error: " << strerror(errno);
        return -1;
    }
    if(ret == 0)
    {
        LOG(INFO) << "Read EOF";
        return -1;
    }

    //read {ret} bytes of data, write it to output_buffer
    char* dataRead = static_cast<char*>(::calloc(recv_buf_size, 1));

    auto len = this->read(dataRead, ret);
    LOG(INFO) << "Recived: " << dataRead;

    using namespace boost::beast::http;
    request_parser<string_body> parser{};

    using namespace boost::asio;

    boost::asio::mutable_buffer buf{static_cast<void*>(dataRead), len};

    boost::beast::error_code ec;
    parser.put(buf, ec);

    boost::beast::http::response<string_body> resp{};
    resp.version(11);
    resp.result(status::ok);
    resp.set(field::server, "unp");

    if(parser.is_done())
    {
        auto message = parser.get();
        auto method = message.method();
        if(method == boost::beast::http::verb::get)
        {
            resp.body() = "Hello World!";
        }else
        {
            resp.body() = "No this Method";
        }
    }else
    {
        resp.body() = "Error";
    }

    resp.prepare_payload();

    serializer<false, string_body> seri{resp};

    WriteLambda writer{*this};
    seri.next(ec, writer);

    ::free(dataRead);

    return 0;
}
