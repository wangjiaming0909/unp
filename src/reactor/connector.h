#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "net/sock_stream.h"
#include "reactor/EventHandler.h"
#include "net/sock_connector.h"
#include "thread/thread_pool.h"
#include "util/easylogging++.h"
#include "reactor/sock_connection_handler.h"
#include "net/unp.h"
#include "reactor/service.h"

#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>

namespace reactor
{

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
  virtual ~connector() override
  {
    delete this->handlerPtr_;
    this->handlerPtr_ = nullptr;
  }
  virtual ConnectionHandlerPtr_t connect(const net::inet_addr &target_addr, micro_seconds timeout) override;
  virtual int disconnect(micro_seconds timeout) override;
  bool is_connected() const { return is_connected_; }
private:
  bool is_connected_ = false;
};

template <typename Handler_t>
typename connector<Handler_t>::ConnectionHandlerPtr_t connector<Handler_t>::connect(const net::inet_addr& target_addr, micro_seconds timeout)
{
  auto *stream = dynamic_cast<net::SockStream*>(this->handlerPtr_->get_stream());
  if (!stream) {
    return nullptr;
  }
  if (this->connector_.connect(*stream, target_addr, &timeout, 1, 0) != 0) {
    LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
    return nullptr;
  }
  is_connected_ = true;

  if(this->handlerPtr_->open() < 0) {
    LOG(WARNING) << "activate connection handler error";
    is_connected_ = false;
    return nullptr;
  }
  return this->handlerPtr_;
}

template <typename Handler_t>
int connector<Handler_t>::disconnect(micro_seconds)
{
  this->handlerPtr_->close();
  is_connected_ = true;
  return 0;
}
} // namespace reactor

#endif // _UNP_REACTOR_CONNECTOR_H_
