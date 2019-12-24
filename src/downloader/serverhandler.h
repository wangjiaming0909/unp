#include  "reactor/connection_handler.h"
#include "reactor/reactor.h"

namespace downloader
{

class ServerHandler : public reactor::connection_handler
{

public:
    ServerHandler(reactor::Reactor& react) : connection_handler(react)
    {
    }
    ~ServerHandler(){}

    virtual int handle_input(int handle) override
    {
    }
    virtual int open() override
    {
    }

};
}
