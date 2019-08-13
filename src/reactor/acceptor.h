#ifndef _UNP_REACTOR_ACCEPTOR_H_
#define _UNP_REACTOR_ACCEPTOR_H_
#include "reactor/EventHandler.h"
#include "net/sock_acceptor.h"
#include "net/inet_addr.h"
#include "reactor/reactor.h"
#include "reactor/read_handler.h"
// #include "reactor/connection_handler.h"
#include "reactor/echo_connection_handler.h"
#include "net/unp.h"
#include <memory>

namespace reactor
{

enum
{
    NOT_REUSE_ADDR = 0,
    REUSE_ADDR = 1
};

enum AcceptorStateEnum
{
    REGISTERED, // means that listen fd has opened, and have also registered into the reactor
    LISTEN_CLOSED, // unregistered the event in the reactor
    ALL_CLOSED
};

class acceptor : public EventHandler
{
public:
    using connection_handler_ptr_type = std::shared_ptr<connection_handler>;

public:
    acceptor(Reactor &react, const net::inet_addr &local_addr);
    //close listen, close all read_handelrs
    int destroy_acceptor();

private:
    virtual ~acceptor() override;

public:
    //new connection coming, make a conenction_handler, activate it
    virtual int handle_input(int handle) override;
    virtual int handle_close(int handle) override;
    virtual int get_handle() const override { return sock_acceptor_.get_handle(); }
    //open a socket, bind to local_addr, listen, acceptor, and register to reactor
    int open();
    //是否应该暴露 close 接口
    //unregister listen event from reactor, close the listen fd
    //不关闭 read_handler, 当需要关闭时, 自己调用 close_all
    int close();
    //connection_handler do not need acceptor to close it, reactor will close it when the connection is over
    //so here just remove all handler from read_handlers
    void close_read_handler(int handle);
    int close_all_handlers();
    void set_external_reactors_(std::vector<std::shared_ptr<Reactor>> external_reactors)
    {
        external_reactors_ = external_reactors;
    }

TEST_PRIVATE:
    //make a read_handler, insert into the vector
    //reactor_to_register specify the reactor that the new connection_handler will register on
    int make_read_handler(Reactor &reactor_to_register);
    int activate_read_handler(int handle);
    void increase_current_reactor_index();

TEST_PRIVATE:
    net::sock_acceptor sock_acceptor_;
    net::inet_addr local_addr_;
    std::vector<connection_handler_ptr_type> read_handlers_;

TEST_PRIVATE:
    //external_reactors_ are reactors that the new connection_handlers will register on
    //并且会轮流register, 如果可以做到: 查看 这些 reactor上的当前事件有多少,然后针对事件比较少的进行register就更好了
    size_t current_reactor_index_to_register_;
    std::vector<std::shared_ptr<Reactor>> external_reactors_;
    AcceptorStateEnum acceptorState_;
    size_t connectionCount_{0};
};

/*
    1, reactor_acceptor has a inet_addr to identify the interface and port of listen fd
    2, reactor_acceptor do not need a set_handle() function, because the sock_acceptor already has a handle in it
    3, reactor_acceptor do not need to override output, because an acceptor do not need to write anything
    4, reactor_acceptor need to create a EventHandler of io_handler to handle the I/O events of the accepted fd
*/
class reactor_acceptor : public EventHandler
{
public:
    //here use a inet_addr,
    //so that we can specify an interface and a port to listen
    reactor_acceptor(Reactor &react, thread_pool &pool, const net::inet_addr &local_addr);
    ~reactor_acceptor() override;
    virtual int handle_input(int handle) override;
    //accept do not need output
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) noexcept override;
    virtual int handle_close(int handle) override;
    virtual int handle_signal(int handle) override;
    virtual int get_handle() const override { return acceptor_.get_handle(); }

private:
    void activate_read_handler();
    int open();
    int close() { return acceptor_.close(); }

private:
    net::sock_acceptor acceptor_;
    net::inet_addr local_addr_;
    //if acceptor has a message_queue, then every ReadHandler will share the same mq
    message_queue<int> mq_;
    thread_pool &pool_;
    ReadHandler<int> read_handler_;
    std::vector<std::shared_ptr<ReadHandler<int>>> read_handlers_; //for multi read_handler
};

template <typename Handler_t>
class Acceptor : public EventHandler
{
public:
    using connection_handler_ptr_type = std::shared_ptr<connection_handler>;

public:
    Acceptor(Reactor &react, const net::inet_addr &local_addr)
        : EventHandler(react), sock_acceptor_(local_addr), local_addr_(local_addr), read_handlers_(1024), external_reactors_()
    {
        static_assert(std::is_base_of<connection_handler, Handler_t>::type, "Handler_t should derive from ConnectionHandler");
        this->current_reactor_index_to_register_ = 0;
    }
    //close listen, close all read_handelrs
    int destroy_acceptor()
    {
        close();
        for (size_t i = 0; i < read_handlers_.size(); i++)
        {
            if (read_handlers_[i] != nullptr)
            {
                LOG(WARNING) << "There are still connection active handle: " << i;
                return -1;
            }
        }
        delete this;
        return 0;
    }

private:
    virtual ~Acceptor() override
    {
        close_all_handlers();
        close();
    }

public:
    //new connection coming, make a conenction_handler, activate it
    virtual int handle_input(int handle) override
    {
        if (handle == INVALID_HANDLE || handle != sock_acceptor_.get_handle())
        {
            LOG(ERROR) << "Handler error...";
            return -1;
        }

        int read_handle = INVALID_HANDLE;
        if (this->external_reactors_.size() == 0)
        {
            read_handle = make_read_handler(*reactor_);
        }
        else
        {
            read_handle = make_read_handler(*external_reactors_[current_reactor_index_to_register_]);
            increase_current_reactor_index();
        }
        return activate_read_handler(read_handle);
    }
    virtual int handle_close(int handle) override
    {
        if (handle == INVALID_HANDLE || handle != sock_acceptor_.get_handle())
        {
            LOG(ERROR) << "Handler error...";
            return -1;
        }

        return close();
    }
    virtual int get_handle() const override { return sock_acceptor_.get_handle(); }
    //open a socket, bind to local_addr, listen, acceptor, and register to reactor
    int open()
    {
        if (sock_acceptor_.open(local_addr_) != 0)
        {
            LOG(ERROR) << "Opening a socket error..." << strerror(errno);
            return -1;
        }

        int ret = reactor_->register_handler(sock_acceptor_.get_handle(), this, ACCEPT_EVENT);
        acceptorState_ = REGISTERED;
        return ret;
    }
    //是否应该暴露 close 接口
    //unregister listen event from reactor, close the listen fd
    //不关闭 read_handler, 当需要关闭时, 自己调用 close_all
    int close()
    {
        if(acceptorState_ == LISTEN_CLOSED || acceptorState_ == ALL_CLOSED) return 0;

        int ret = reactor_->unregister_handler(sock_acceptor_.get_handle(), this, ACCEPT_EVENT);

        ret = sock_acceptor_.close() && ret;
        if(ret != 0)
        {
            LOG(ERROR) << "Close acceptor error..." << strerror(errno);
        }

        if(connectionCount_ == 0) acceptorState_ = ALL_CLOSED;
        else acceptorState_ = LISTEN_CLOSED;

        return ret;
    }
    //connection_handler do not need acceptor to close it, reactor will close it when the connection is over
    //so here just remove all handler from read_handlers
    void close_read_handler(int handle)
    {
        if (handle < 0 || static_cast<size_t>(handle) > read_handlers_.size())
        {
            LOG(ERROR) << "Close read Handler error, handle: " << handle;
        }
        //一般也不会有别人会获得这些 connection_handler 的指针,因此 reset 之后就会析构此 connection_handler
        // read_handlers_[handle]->close();
        read_handlers_[handle].reset();
        connectionCount_--;
    }

    int close_all_handlers()
    {
        connectionCount_ = 0;
        return 0;
    }
    void set_external_reactors_(std::vector<std::shared_ptr<Reactor>> external_reactors)
    {
        external_reactors_ = external_reactors;
    }

TEST_PRIVATE:
    //make a read_handler, insert into the vector
    //reactor_to_register specify the reactor that the new connection_handler will register on
    int make_read_handler(Reactor &reactor_to_register)
    {
        if (read_handlers_.size() >= INT32_MAX)
        {
            LOG(WARNING) << "Too many connections... ";
            return -1;
        }

        std::shared_ptr<connection_handler> handler{new Handler_t{reactor_to_register}};
        handler->set_closed_callback(std::bind(&acceptor::close_read_handler, this, std::placeholders::_1));

        net::inet_addr peer_addr{};

        int ret = sock_acceptor_.accept(handler->get_sock_stream(), &peer_addr);
        if (ret != 0)
        {
            LOG(ERROR) << "Acceptor error..." << strerror(errno);
            return -1;
        }

        int handle = handler->get_handle();
        LOG(INFO) << "connection from: " << peer_addr.get_address_string() << " handle: " << handle;
        if (handle < 0)
        {
            LOG(ERROR) << "Handler errror" << strerror(errno);
            return handle;
        }

        // if(read_handlers_.size() <= static_cast<size_t>(handle)) read_handlers_.resize(handle + 10);
        read_handlers_[static_cast<uint32_t>(handle)] = handler;
        connectionCount_++;

        return handle;
    }

    int activate_read_handler(int handle)
    {
        assert(handle >= 0);
        auto &handler = read_handlers_[static_cast<uint32_t>(handle)];

        return handler->open();
    }

    void increase_current_reactor_index()
    {
        if (current_reactor_index_to_register_ == external_reactors_.size() - 1)
        {
            current_reactor_index_to_register_ = 0;
        }
        else
        {
            current_reactor_index_to_register_++;
        }
    }

TEST_PRIVATE:
    net::sock_acceptor sock_acceptor_;
    net::inet_addr local_addr_;
    std::vector<connection_handler_ptr_type> read_handlers_;

TEST_PRIVATE:
    //external_reactors_ are reactors that the new connection_handlers will register on
    //并且会轮流register, 如果可以做到: 查看 这些 reactor上的当前事件有多少,然后针对事件比较少的进行register就更好了
    size_t current_reactor_index_to_register_;
    std::vector<std::shared_ptr<Reactor>> external_reactors_;
    AcceptorStateEnum acceptorState_;
    size_t connectionCount_{0};
};


} // namespace reactor
#endif // _UNP_REACTOR_ACCEPTOR_H_
