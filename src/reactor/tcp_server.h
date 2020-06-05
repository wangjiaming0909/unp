#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "thread/thread_pool.h"
#include "reactor/acceptor.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/poll_reactor_impl.h"
#include "reactor/epoll_reactor_impl.h"
#include "net/inet_addr.h"
#include "net/unp.h"
#include <memory>

namespace reactor
{

class Reactor;

/*
    reactor的个数应该与 thread 的个数相同，因为每个thread里会放一个reactor
    其中，处理listen事件的线程为第一线程，此线程会在 reactor上放 acceptor

    TODO 如何某个线程的reactor正处于poll_wait 状态，那么当有新的连接到达时，假如正好被注册到此reactor上，那么如何唤醒此reactor，并注册上去呢?
        1, 一种方案是： 将每一个reactor上都注册一个自己造的事件，用此事件来唤醒reactor，当更改了demultiplex_table的时候，就往该伪造的fd上写数据，唤醒reactor
            并且有此fd以及它对应的事件可以防止首次将此reactor放入某个线程时该线程不阻塞， 因为可能并没有事件在此reactor上注册
 			可以使用eventfd 或者pipe
        2, 还有其他的方法么, 可以使用signal 中断poll等系统调用,然后在poll的返回时重新启动
 * epoll中可以直接在其他线程添加events,并且epoll_wait的线程也会得到此消息,见man page
 * 	但是如此的话当把epoll的reactor添加到线程中的时候,并不会阻塞,因为可能还没有事件注册
 * 	给epoll 的 reactor 添加 锁和 condition_variable, 没有事件时,等待在 condition_variable 上
 * select和poll并不具有此功能,因此可以使用以上的两种中的一种

    TODO reactor内部的线程安全性问题
        reactor 的register, unregister等操作都是在第一线程中完成的
        而handle_events操作都是在其对应的线程中完成的
    TODO sock_connection_handler 内部的线程安全性问题
        sock_connection_handler 的get_handle, open, enable_reading等方法都是在第一线程中调用的(在reactor内部)
        而 handle_input, handle_output 都是在对应的reactor线程中被调用的
*/

template <typename Handler_t>
class tcp_server
{
public:
    using Acceptor_t = Acceptor<Handler_t>;
    using reactor_ptr_t	= std::shared_ptr<Reactor>;
    using acceptor_ptr_t = Acceptor_t*;
    using pool_ptr_t = std::shared_ptr<thread::thread_pool>;
    static const int DEFAULT_THREAD_NUM = 4;
public:
    tcp_server(const net::inet_addr& local_addr);
    ~tcp_server();

public:
    //must call before open
    void set_thread_num(size_t n);

    int start(unp::reactor_imp_t_enum impl);
    int suspend();
    int stop(bool force);

TEST_PRIVATE:
	reactor_ptr_t make_reactor(unp::reactor_imp_t_enum reactor_t);
    void make_acceptor();
    void suspendAcceptorReactor();
    void suspendConnectionReactor();

TEST_PRIVATE:
    size_t thread_num_ = DEFAULT_THREAD_NUM;

TEST_PRIVATE:
    reactor_ptr_t first_reactor_;
    acceptor_ptr_t acceptor_;
    pool_ptr_t pool_;
    net::inet_addr local_addr_;

    std::vector<reactor_ptr_t> connection_reactors_;
};

template <typename Handler_t>
tcp_server<Handler_t>::tcp_server(const net::inet_addr& local_addr)
    : first_reactor_(0)
    , acceptor_(0)
    , pool_()
    , local_addr_(local_addr)
    , connection_reactors_() { }

template <typename Handler_t>
tcp_server<Handler_t>::~tcp_server()
{
    if(acceptor_) acceptor_->destroy_acceptor();
}

template <typename Handler_t>
int tcp_server<Handler_t>::start(unp::reactor_imp_t_enum impl)
{
    LOG(INFO) << "Starting server on " << local_addr_.get_address_string();
    first_reactor_ = make_reactor(impl);
    if(first_reactor_.get() == nullptr)
        return -1;

    make_acceptor();
    if(acceptor_->open() != 0)
    {
        return -1;
    }
    pool_ = std::make_shared<thread::thread_pool>(thread_num_);

    for(size_t i = 0; i < thread_num_; i++)
    {
        auto connection_reactor = make_reactor(impl);
        if(connection_reactor.get() == nullptr) return -1;
        connection_reactors_.push_back(connection_reactor);
        auto current_reactor = connection_reactors_.back();
        pool_->add_task(
        [=]()
        {
            while(true)
            {
                auto ret = current_reactor->handle_events();
                if(ret != 0)
                {
                    LOG(ERROR) << "handle events error";
                    break;
                }
            }
        });
    }
    if(thread_num_ > 0)
        // acceptor_->set_external_reactors_(connection_reactors_);

    // pool_->start();
    while(1)
    {
        auto ret = first_reactor_->handle_events();
        if(ret < 0)
        {
            LOG(ERROR) << "handle events error";
            break;
        }
    }

    std::chrono::microseconds timeout = 2s;
    pool_->wait(&timeout);
    return -1;
}

template <typename Handler_t>
int tcp_server<Handler_t>::suspend()
{
    suspendAcceptorReactor();
    suspendConnectionReactor();
    return 0;
}

template <typename Handler_t>
void tcp_server<Handler_t>::suspendAcceptorReactor()
{
    first_reactor_->suspend();
}

template <typename Handler_t>
void tcp_server<Handler_t>::suspendConnectionReactor()
{
    for(auto connReactor : connection_reactors_)
    {
        connReactor->suspend();
    }
}

template <typename Handler_t>
int tcp_server<Handler_t>::stop(bool force)
{
    LOG(INFO) << "closing tcp_server...";
    suspend();
    //TODO wait until all the reactors are suspended
    if(!force)
    {
        return acceptor_->destroy_acceptor();
    }
    if(acceptor_->destroy_acceptor() != 0)
    {
        acceptor_->close_all_handlers();
    }
    return acceptor_->destroy_acceptor();
}

template <typename Handler_t>
typename tcp_server<Handler_t>::reactor_ptr_t tcp_server<Handler_t>::make_reactor(unp::reactor_imp_t_enum reactor_t)
{
	reactor_ptr_t ret_ptr;
	Reactor* reactor_ret = nullptr;
    switch(reactor_t)
    {
        case unp::reactor_imp_t_enum::USING_SELECT:
            reactor_ret = new Reactor(new select_reactor_impl{}, true);
            break;
        case unp::reactor_imp_t_enum::USING_POLL:
            reactor_ret = new Reactor(new poll_reactor_impl{}, true);
            break;
        case unp::reactor_imp_t_enum::USING_EPOLL:
            reactor_ret = new Reactor(new epoll_reactor_impl{}, true);
            break;
        default:
			break;
    }
    ret_ptr.reset(reactor_ret);
	return ret_ptr;
}

template <typename Handler_t>
void tcp_server<Handler_t>::make_acceptor()
{
    acceptor_ = new Acceptor_t(*first_reactor_, local_addr_);
}

template <class Handler_t>
void tcp_server<Handler_t>::set_thread_num(size_t n)
{
    if(n == 0 || n > 32)
    {
        LOG(WARNING) << "thread num can't be large than 32 or can't be 0: " << n;
        n = DEFAULT_THREAD_NUM;
    }

    thread_num_ = n;
}


} //namespace reactor
#endif // TCP_SERVER_H
