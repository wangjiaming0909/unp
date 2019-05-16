#include "server/reactor/acceptor.h"

using namespace reactor;

reactor_acceptor:: reactor_acceptor(
    Reactor& react, thread_pool& pool,
    const net::inet_addr& local_addr)
        : event_handler(react)
        , acceptor_(local_addr)
        , local_addr_(local_addr)
        , mq_()
        , pool_(pool)
        , read_handler_(react, pool_, mq_)
        , read_handlers_() { this->open(); }

reactor_acceptor::~reactor_acceptor(){ }

int reactor_acceptor::open(){
    int ret = 0;
    if((ret = acceptor_.open(local_addr_, REUSE_ADDR)) < 0){
        LOG(ERROR) << "sock_acceptor open error..." << strerror(errno);
        return ret;
    }
    //open succeed, register to the reactor
    return reactor_->register_handler(acceptor_.get_handle(), this, event_handler::ACCEPT_EVENT);
}

int reactor_acceptor::handle_input(int handle){
    LOG(INFO) << "start to accept a connection";
    // pool_.start();
    do{
        activate_read_handler();
    }while(unp::handle_read_ready_using_poll(handle, 2ms) == 1);
    // read_handler_.activate(1);
    return 0;
}

void reactor_acceptor::activate_read_handler(){
    net::inet_addr peer{};
    ReadHandler<int> tmpHandler{*read_handler_.get_reactor(), *read_handler_.pool(), *read_handler_.mq()};
    int ret = this->acceptor_.accept(tmpHandler.get_sock_stream(), &peer);
    if(ret == 0) 
        LOG(INFO) << "accepted...";
    if(ret != 0){
        LOG(ERROR) << "accept error..." << strerror(errno);
    }

    int handle = tmpHandler.get_handle();
    if(read_handlers_.size() < static_cast<size_t>(handle + 1)){
        read_handlers_.resize(handle + 1);
    }

    read_handlers_[handle].reset(new ReadHandler<int>(std::move(tmpHandler)));
    if(ret != 0 && read_handlers_[handle]->get_sock_stream().get_sock_fd().set_non_blocking() != 0)
        LOG(WARNING) << "setting nonblock error....";
    read_handlers_[handle]->open();
    // if(ret != 0 && read_handler_.get_sock_stream().get_sock_fd().set_non_blocking() != 0){
    //     LOG(WARNING) << "setting nonblock error";
    // }
    // read_handler_.open();
}


int reactor_acceptor::handle_timeout(int){ return 0;}
int reactor_acceptor::handle_close(int){ return 0; }
int reactor_acceptor::handle_signal(int){ return 0;}



acceptor::acceptor(Reactor& react, const net::inet_addr& local_addr)
    : event_handler(react)
    , sock_acceptor_(local_addr)
    , local_addr_(local_addr)
    , read_handlers_(1024)
	, external_reactors_()
{
//    open();
	this->current_reactor_index_to_register_ = 0;
}

int acceptor::destroy_acceptor()
{
    close();
    for (size_t i = 0; i < read_handlers_.size(); i++) {
        if(read_handlers_[i] != nullptr)
        {
            LOG(WARNING) << "There are still connection active handle: " << i;
            return -1;
        }
    }
    delete this;
    return 0;
}

acceptor::~acceptor(){
    close_all_handlers();
    close();
}

int acceptor::handle_input(int handle)
{
    if (handle == INVALID_HANDLE || handle != sock_acceptor_.get_handle())
    {
        LOG(ERROR) << "Handler error...";
        return -1;
    }

	int read_handle = INVALID_HANDLE;
	if(this->external_reactors_.size() == 0)
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

void acceptor::increase_current_reactor_index()
{
	if(current_reactor_index_to_register_ == external_reactors_.size() - 1)
	{
		current_reactor_index_to_register_ = 0;
	}
	else
	{
		current_reactor_index_to_register_++;
	}
}

int acceptor::handle_close(int handle)
{
    if(handle == INVALID_HANDLE || handle != sock_acceptor_.get_handle())
    {
        LOG(ERROR) << "Handler error...";
        return -1;
    }

    return close();
}

int acceptor::open()
{
    if(sock_acceptor_.open(local_addr_) != 0)
    {
        LOG(ERROR) << "Opening a socket error..." << strerror(errno);
        return -1;
    }

    return reactor_->register_handler(sock_acceptor_.get_handle(), this, ACCEPT_EVENT);
}

int acceptor::close()
{
    if(sock_acceptor_.close() != 0)
    {
        LOG(ERROR) << "Close a socket error..." << strerror(errno);
        return -1;
    }

    return reactor_->unregister_handler(sock_acceptor_.get_handle(), this, ACCEPT_EVENT);
}

void acceptor::close_read_handler(int handle)
{
    if(handle < 0 || static_cast<size_t>(handle) > read_handlers_.size())
    {
        LOG(ERROR) << "Close read Handler error, handle: " << handle;
    }
	//一般也不会有别人会获得这些 connection_handler 的指针,因此 reset 之后就会析构此 connection_handler
	// read_handlers_[handle]->close();
    read_handlers_[handle].reset();
}

int acceptor::close_all_handlers()
{
	return 0;
}

int acceptor::make_read_handler(Reactor& reactor_to_register)
{
    if(read_handlers_.size() >= INT32_MAX)
    {
        LOG(WARNING) << "Too many connections... ";
        return -1;
    }

    std::shared_ptr<connection_handler> handler{new echo_connection_handler{reactor_to_register}};
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
    if(handle < 0)
    {
        LOG(ERROR) << "Handler errror" << strerror(errno);
        return handle;
    }

    // if(read_handlers_.size() <= static_cast<size_t>(handle)) read_handlers_.resize(handle + 10);
    read_handlers_[static_cast<uint32_t>(handle)] = handler;

    return handle;
}

int acceptor::activate_read_handler(int handle)
{
    assert(handle >= 0);
    auto& handler = read_handlers_[static_cast<uint32_t>(handle)];

    return handler->open();

    // if(handler->get_sock_stream().get_sock_fd().set_non_blocking() != 0)
    // {
    //     LOG(ERROR) << "Set nonblock error, handle : " << handle << " error: " << strerror(errno);
    //     return -1;
    // }

    // if(handler->enable_reading() != 0)
    // {
    //     LOG(ERROR) << "Registering error: " << strerror(errno);
    //     return -1;
    // }
    // return 0;
}
