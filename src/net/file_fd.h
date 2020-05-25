#include "net/fd.h"
#include "net/unp.h"
#include <string>

namespace unp
{
class FileFD : public fd
{
public:
  FileFD();
  FileFD(const char* file_path);
  ~FileFD();
  virtual int open() = 0;
  virtual int close(int how) = 0;
TEST_PRIVATE:
  std::string file_path_;
};
}
