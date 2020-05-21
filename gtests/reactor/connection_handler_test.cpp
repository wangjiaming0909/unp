#include <gtest/gtest.h>
#include <thread>
#include "reactor/sock_connection_handler.h"
#include "reactor/epoll_reactor_impl.h"
#include "reactor/reactor.h"

using namespace reactor;
TEST(sock_connection_handler, test_thread_safety_of_output_buffer)
{
  reactor::Reactor react(new reactor::epoll_reactor_impl());
  sock_connection_handler handler(react);

  char* d = (char*)::calloc(1024, 1);

  handler.write(d, 1024, false);
  handler.write(d, 1024, false);
  handler.write(d, 1024, false);
  handler.write(d, 1024, false);

  std::thread t([&]()
      {
        for(int i = 0; i < 10000; i++) {
          handler.write(d, 16, false);
        }
      });

  std::thread t2([&]()
      {
        while(handler.output_buffer_.buffer_length() > 0)
        {
          handler.drain_output_buffer(16);
        }
      });
  t.join();
  t2.join();
}
