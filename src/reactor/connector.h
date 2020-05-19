#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "reactor/EventHandler.h"
#include "net/sock_connector.h"
#include "thread/thread_pool.h"
#include "util/easylogging++.h"
#include "reactor/connection_handler.h"
#include "boost/intrusive/list_hook.hpp"
#include "net/unp.h"

#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>

namespace reactor
{

using IntrusiveListBaseHook = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
struct ServiceT : public IntrusiveListBaseHook
{
    virtual ~ServiceT() = default;
    virtual int open() = 0;
    virtual int close() = 0;
};


template <typename Handler_t>
struct IConnector : public ServiceT
{
    using micro_seconds = std::chrono::microseconds;
    using HandlerT = Handler_t  ;
    using ConnectionHandlerPtr_t = Handler_t*;

    IConnector(ConnectionHandlerPtr_t handlerP) : connector_{}, handlerPtr_{handlerP}{}
    virtual Handler_t* connect(const net::inet_addr& targetAddr, micro_seconds timeout) = 0;
    virtual int disconnect(micro_seconds timeout = std::chrono::microseconds{1}) = 0;
    int open() override {return 0;}
    int close() override { disconnect(); return 0;}
    virtual ~IConnector() = default;
    ConnectionHandlerPtr_t get_connection_handler() {return handlerPtr_;}
TEST_PROTECTED:
    net::sock_connector connector_;
    ConnectionHandlerPtr_t handlerPtr_;
};

template <typename Handler_t >
class connector : public IConnector<Handler_t>
{
public:
    using Base_t = IConnector<Handler_t>;
    using micro_seconds = typename Base_t::micro_seconds;
    using HandlerT = typename Base_t::HandlerT;
    using ConnectionHandlerPtr_t = typename Base_t::ConnectionHandlerPtr_t;

    connector(Handler_t& handler) : Base_t(&handler) { }
    virtual ~connector() override { delete this->handlerPtr_;}

    virtual ConnectionHandlerPtr_t connect(const net::inet_addr &target_addr, micro_seconds timeout) override;
    virtual int disconnect(micro_seconds timeout) override;
};

template <typename Handler_t>
typename connector<Handler_t>::ConnectionHandlerPtr_t connector<Handler_t>::connect(const net::inet_addr& target_addr, micro_seconds timeout)
{
    auto &stream = this->handlerPtr_->get_sock_stream();

    if (this->connector_.connect(stream, target_addr, &timeout, 1, 0) != 0)
    {
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        return nullptr;
   }

   if(this->handlerPtr_->open() < 0)
    {
        LOG(WARNING) << "activate connection handler error";
        return nullptr;
    }
    return this->handlerPtr_;
}

template <typename Handler_t>
int connector<Handler_t>::disconnect(micro_seconds)
{
    this->handlerPtr_->close();
    return 0;
}
} // namespace reactor

#endif // _UNP_REACTOR_CONNECTOR_H_
