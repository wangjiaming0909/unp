#ifndef _UNP_FILEUTIL_H_
#define _UNP_FILEUTIL_H_
#include <string>
#include <errno.h>

namespace util{
  using std::string;

class FileUtil{
public:
    FileUtil(const string &fileName);
    ~FileUtil();

public:
    // **return errno
    int readToString(int maxSize, string* str_ptr);
    // **read to the m_buf
    // int readIn(size_t* size);
    const char* buffer() const {return m_buf;}

public:
    static const int kBufferSize = 64*1024;

private:
    bool fd_is_valid(std::string* str_ptr) const;
private:
    int     		m_fd;
    int     		m_err;
    char    	    m_buf[kBufferSize];
};

}

#endif // _FILEUTIL_H_
