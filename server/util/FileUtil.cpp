#include "FileUtil.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

util::FileUtil::FileUtil(const util::string& fileName)
    : m_fd(::open(fileName.ptr(), O_RDONLY | O_CLOEXEC)),
    m_err(0){
    memset(m_buf, 0, kBufferSize);
    if(m_fd < 0)
        m_err = errno;
}

util::FileUtil::~FileUtil(){
    if(m_fd > 0)
        ::close(m_fd);
}

int util::FileUtil::readToString(int maxSize, util::string* str_ptr){
    assert(str_ptr != NULL);
    int err = m_err;
    int fileSize = 0;
    if(m_fd >= 0){
        str_ptr->clear();
        struct stat statbuf;
        if(::fstat(m_fd, &statbuf) == 0){
            if(fileSize = statbuf.st_size){// ** file size unit is byte
                if(S_ISREG(statbuf.st_mode)){// **normal file
                    if(statbuf.st_size > kBufferSize){
                        exit(-1);
                    }
                }else if(S_ISDIR(statbuf.st_mode)){// ** directory
                    err = EISDIR;
                }
            }
        }
    }
    while(str_ptr->size() < maxSize){
        int toRead = std::min(maxSize - str_ptr->size(), 64*1024);
        ssize_t n = ::read(m_fd, m_buf, toRead);
        if(n > 0)
            ;// str_ptr->append(m_buf, n);
        else{
            if(n < 0)
                err = errno;
            break;
        }
    }
    return err;
}

int util::FileUtil::readIn(int* size){//? in and out (size)

}