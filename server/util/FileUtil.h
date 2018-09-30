#ifndef _FILEUTIL_H_
#define _FILEUTIL_H_
#include "XString.h"

namespace util{

class FileUtil{
public:
    FileUtil(const string &fileName);
    ~FileUtil();

public:
    // **return errno
    int readToString(int maxSize, string* str_ptr);
    // **read to the m_buf
    int readIn(int* size);
    const char* buffer() const {return m_buf;}

public:
    static const int kBufferSize = 64*1024;
private:
    int     m_fd;
    int     m_err;
    char    m_buf[kBufferSize];
};

}

#endif // _FILEUTIL_H_
