#include "tcp_server.h"
#include "select_reactor_impl.h"
#include "poll_reactor_impl.h"
#include "server/util/easylogging++.h"

namespace reactor
{

tcp_server::tcp_server(const net::inet_addr& local_addr)
    : reactor_(0)
    , acceptor_(0)
    , pool_()
    , local_addr_(local_addr)
{
}

tcp_server::~tcp_server()
{
	close(true);
}

int tcp_server::open()
{
	auto listen_reactor = make_reactor(reactor_imp_t_enum::USING_EPOLL);
	if(listen_reactor.get() == nullptr)
		return -1;

    make_acceptor();
    pool_ = std::make_shared<thread::thread_pool>(thread_num_);
}

int tcp_server::close(bool force)
{
	if(!force)
	{
		int ret = acceptor_->destroy_acceptor();
	}
}

tcp_server::reactor_ptr_t tcp_server::make_reactor(reactor_imp_t_enum reactor_t)
{
	reactor_ptr_t ret_ptr;
	Reactor* reactor_ret = nullptr;
    switch(reactor_t)
    {
        case reactor_imp_t_enum::USING_SELECT:
            reactor_ret = new Reactor(new select_reactor_impl{});
            break;
        case reactor_imp_t_enum::USING_POLL:
            reactor_ret = new Reactor(new poll_reactor_impl{});
            break;
        case reactor_imp_t_enum::USING_EPOLL:
            reactor_ret = new Reactor(new epoll_reactor_impl{});
            break;
        default:
			break;
    }
    ret_ptr.reset(reactor_ret);
	return ret_ptr;
}

void tcp_server::make_acceptor()
{
    acceptor_ = new acceptor (*reactor_, local_addr_);
}

void tcp_server::set_thread_num(size_t n)
{
    if(n == 0 || n > 32)
    {
        LOG(WARNING) << "thread num can't be large than 32 or can't be 0: " << n;
        n = DEFAULT_THREAD_NUM;
    }

    thread_num_ = n;
}



} //namespace reactor
