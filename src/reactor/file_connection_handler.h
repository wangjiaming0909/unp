#pragma once
#include "reactor/connection_handler.h"

namespace reactor
{
class FileConnectionHandler : public connection_handler
{
public:
  FileConnectionHandler(Reactor& react);
  virtual ~FileConnectionHandler();

  virtual int handle_input(int) override;
  virtual int handle_output(int) override;
  virtual int handle_timeout(int) noexcept override;
  virtual int handle_close(int) override;
  virtual int handle_signal(int) override;
  virtual int get_handle() const override;
  virtual void set_handle(int) override;
  virtual void init_stream() override;

  int open_file(const char* file_path, int flags);
};
}
