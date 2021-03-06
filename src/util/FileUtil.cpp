#include "FileUtil.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <sys/stat.h>
#include <cstdlib>
#include "easylogging++.h"

util::FileUtil::FileUtil(const std::string& fileName)
    : m_fd(::open(fileName.c_str(), O_RDONLY | O_CLOEXEC)),
    m_err(0){
    memset(m_buf, 0, kBufferSize);
    if(m_fd < 0){
        m_err = errno;
        std::string error_message = strerror(errno);
        error_message = error_message + " " + fileName;
        LOG(ERROR) << error_message;
    }
}

util::FileUtil::~FileUtil(){
    if(m_fd > 0)
        ::close(m_fd);
}

bool util::FileUtil::fd_is_valid(std::string* str_ptr)const {
    int err = m_err;
    int fileSize = 0;
    if(m_fd >= 0){
        str_ptr->clear();
        struct stat statbuf;
        if(::fstat(m_fd, &statbuf) == 0){
            fileSize = static_cast<int>(statbuf.st_size);
            if(fileSize){// ** file size unit is byte
                if(S_ISREG(statbuf.st_mode)){// **normal file
                    if(statbuf.st_size > kBufferSize){
                        LOG(WARNING) << "file too big";
                    }
                }else if(S_ISDIR(statbuf.st_mode)){// ** directory
                    LOG(WARNING) << "isDIR";
                    err = EISDIR;
                }
            }
        }
    }
    return err == 0;
}

int util::FileUtil::readToString(int maxSize, std::string* str_ptr){
    assert(str_ptr != nullptr);
    int err = m_err;
    if(!fd_is_valid(str_ptr)){
//        CONSOLE_LOG("fd is not valid");
        LOG(FATAL) << "fd is not valid";
        // exit(-1);
        return -1;
    }
    while(str_ptr->size() < static_cast<size_t>(maxSize)){
        int toRead = std::min(maxSize - static_cast<int>(str_ptr->size()), 64*1024);
        ssize_t n = ::read(m_fd, m_buf, static_cast<size_t>(toRead));
        if(n > 0)
            str_ptr->append(m_buf, n);//? no need to cast
        else{
            if(n < 0)
                err = errno;
            break;
        }
    }
    return err;
}

// int util::FileUtil::readIn(size_t* size){//? in and out (size)
//     size = nullptr;
//     return true;
// }
