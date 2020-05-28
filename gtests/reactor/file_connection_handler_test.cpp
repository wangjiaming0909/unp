#include <gtest/gtest.h>
#include "reactor/epoll_reactor_impl.h"
#include "reactor/file_reader.h"
#include "reactor/file_connection_handler.h"
#include "reactor/reactor.h"
#include "reactor/ConnectionManager.h"

TEST(FileConnectionHandler, a)
{
  using namespace reactor;
  Reactor react{new epoll_reactor_impl{}};
  ConnectionManager manager{react};

  using Reader = FileReader<FileConnectionHandler>;

  auto reader = manager.makeConnection<Reader>();
  ASSERT_TRUE(reader != nullptr);
  reader->open_file("/boost_1_72_0/index.html", O_RDONLY);
  react.handle_events();
}
