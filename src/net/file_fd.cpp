#include "net/file_fd.h"

namespace unp
{
FileFD::FileFD()
{
}

FileFD::FileFD(const char* file_path)
  : file_path_(file_path)
{

}

FileFD::~FileFD()
{
}

int FileFD::open()
{
}

int FileFD::close(int how)
{
}

}
