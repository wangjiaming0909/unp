#include "reactor/file_connection_handler.h"
#include "net/file_stream.h"

namespace reactor
{

FileConnectionHandler::FileConnectionHandler(Reactor &react)
  : connection_handler(react)
{
  init_stream();
  //TODO
}

FileConnectionHandler::~FileConnectionHandler()
{}

int FileConnectionHandler::handle_input(int)
{
}
int FileConnectionHandler::handle_output(int)
{}

int FileConnectionHandler::handle_timeout(int) noexcept
{
}
int FileConnectionHandler::handle_close(int)
{}
int FileConnectionHandler::handle_signal(int)
{}
int FileConnectionHandler::get_handle() const
{}
void FileConnectionHandler::set_handle(int)
{}

void FileConnectionHandler::init_stream()
{
  stream_ = new FileStream();
}

}
