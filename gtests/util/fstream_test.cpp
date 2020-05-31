#include <gtest/gtest.h>
#include <fstream>
#include "util/timer.h"

using namespace std;

class FileWriter
{
public:
  using Size_t =  std::streamsize;
  FileWriter(const char* fileName)
    : fileName_(fileName)
    , fstream_()
  {
    if(fileName != nullptr)
      valid_ = true;
  }

  uint64_t bytesWritten() const{return bytesWritten_;}

  void setBufAndOpen(void* buf, Size_t size)
  {
    if(!valid_)
      return;
    fstream_.rdbuf()->pubsetbuf(static_cast<char*>(buf), size);
    fstream_.open(fileName_, fstream_.out | fstream_.in | fstream_.trunc);
  }

  void write(const char* data)
  {
    bytesWritten_ += strlen(data);
    fstream_ << data;
  }

  FileWriter& write(const char* data, Size_t size)
  {
    if(!valid_)
      return *this;
    auto streamBuf = fstream_.rdbuf();
    streamBuf->sputn(data, size);
    bytesWritten_ += size;

    return *this;
  }

  void flush()
  {
    if(!valid_)
      return;
    fstream_.flush();
  }
  void close()
  {
    fstream_.flush();
    fstream_.close();
  }

private:
  std::string fileName_;
  std::fstream fstream_;
  bool valid_ = false;
  uint64_t bytesWritten_ = 0;
};

TEST(FStream, normal)
{
  GTEST_SKIP();
  const char *fileName = "/tmp/1.txt";
  fstream fs{ fileName, fs.in | fs.binary | fs.out | fs.trunc};
  if(fs.is_open()) {
    for (int i = 0; i < (1024*1024*500 / 20); i++) {
      fs << "current index is: " << i << endl;
      if(20*1024*1024 / 20 < i)
        fs.flush();
    }
    fs.flush();
    fs.close();
  } else {
    FAIL();
  }
}

TEST(FStream, normal2)
{
  GTEST_SKIP();
  {
    utils::timer _{"writing with manual flush"};
    const char* fileName = "/tmp/2.txt";
    void* buf = ::calloc(10*1024*1024, 1);
    FileWriter writer{fileName};
    writer.setBufAndOpen(buf, 10*1024*1024);
    const char* data = "1234567890\n";
    auto size = strlen(data);
    for(int i = 0; i < (1024*1024*20); i++)
      writer.write(data, size);
    writer.close();
    free(buf);
  }
  {
    utils::timer _{"writing with auto flush"};
    const char* fileName = "/tmp/3.txt";
    void *buf = ::calloc(10*1024*1024, 1);
    FileWriter writer{fileName};
    writer.setBufAndOpen(buf, 10*1024*1024);
    for(int i = 0; i < (1024*1024*20); i++)
      writer.write("1234567890\n");
    writer.close();
    free(buf);
  }
}
