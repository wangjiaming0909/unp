#include "tcp_server.h"
#include "select_reactor_impl.h"
#include "poll_reactor_impl.h"
#include "server/util/easylogging++.h"

namespace reactor
{

tcp_server::tcp_server(const net::inet_addr& local_addr)
    : first_reactor_(0)
    , acceptor_(0)
    , pool_()
    , local_addr_(local_addr)
    , connection_reactors_()
{
}

tcp_server::~tcp_server()
{
	close(true);
}

int tcp_server::open()
{
    auto listen_reactor = make_reactor(reactor_imp_t_enum::USING_SELECT);
	if(listen_reactor.get() == nullptr)
		return -1;
    first_reactor_.swap(listen_reactor);

    make_acceptor();
	if(acceptor_->open() != 0)
	{
		return -1;
	}
    pool_ = std::make_shared<thread::thread_pool>(thread_num_);

    for(size_t i = 0; i < thread_num_; i++)
    {
        auto connection_reactor = make_reactor(reactor_imp_t_enum::USING_POLL);
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
        acceptor_->set_external_reactors_(connection_reactors_);

    pool_->start();
    int i = 10;
    while(1)
    {
        auto ret = first_reactor_->handle_events();
        if(ret != 0)
        {
            LOG(ERROR) << "handle events error";
            break;
        }
        i--;
    }

	std::chrono::microseconds timeout = 2s;
	pool_->wait(&timeout);

    return -1;
}

int tcp_server::close(bool force)
{
	if(!force)
	{
		acceptor_->destroy_acceptor();
	}
    
    return acceptor_->destroy_acceptor();
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
    acceptor_ = new acceptor (*first_reactor_, local_addr_);
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
