#include "JJHandler.h"
#include "reactor/reactor.h"


namespace examples
{
JJHandler::JJHandler(Reactor& react, bool isSSL)
  : sock_connection_handler(react, isSSL)
{
}

JJHandler::~JJHandler()
{
}
}
