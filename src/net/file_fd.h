#include "net/fd.h"
#include "net/unp.h"
#include <string>

namespace unp
{
class FileFD : public fd
{
public:
  FileFD();
  FileFD(const char* file_path, int flags);
  ~FileFD();
  void set_open_flags(int flags) {open_flags_ = flags;}
  virtual int open() override;
  virtual int close(int how) override;
TEST_PRIVATE:
  std::string file_path_;
  int open_flags_;
};
}
