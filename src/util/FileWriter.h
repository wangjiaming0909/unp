#pragma once
#include "boost/noncopyable.hpp"
#include <fstream>

namespace utils
{
class FileWriter : boost::noncopyable
{
public:
    using Size_t =  std::streamsize;
    FileWriter(const char* fileName)
        : fileName_(fileName)
        , fstream_()
    {
        if(fileName != nullptr) valid_ = true;
    }

    ~FileWriter(){
        if(isUsingDefautBuf) free(defaultBuf_);
    }

    int resetFileName(const std::string& fileName)
    {
        if(bytesWritten_ > 0)
        {
            return -1;
        }
        fileName_ = fileName;
        return 0;
    }

    bool isValid() const
    {
        return valid_;
    }

	uint64_t bytesWritten() const {return bytesWritten_;}

    void setBufAndOpen(void* buf, Size_t size)
    {
        bytesWritten_ = 0;
        if (!valid_ || isUsingConstomizedBuf_ || isUsingDefautBuf) return;
        fstream_.rdbuf()->pubsetbuf(static_cast<char*>(buf), size);
        fstream_.open(fileName_, fstream_.out | fstream_.in | fstream_.trunc);
        isUsingConstomizedBuf_ = true;
    }

    FileWriter& write(const char* data, Size_t size)
    {
        if(!isUsingConstomizedBuf_ && !isUsingDefautBuf) setDefaultBuffer();
        if(!valid_) return *this;
        auto streamBuf = fstream_.rdbuf();
        auto s = streamBuf->sputn(data, size);
        bytesWritten_ += s;
        return *this;
    }


    void flush()
    {
        if(!valid_) return;
        fstream_.flush();
    }
    void close()
    {
        fstream_.flush();
        fstream_.close();
    }

private:
    void setDefaultBuffer()
    {
        if(!valid_) return;
        defaultBuf_ = ::calloc(DEFAULTBUFFERSIZE, 1);
        fstream_.rdbuf()->pubsetbuf(static_cast<char*>(defaultBuf_), DEFAULTBUFFERSIZE);
        fstream_.open(fileName_, fstream_.out | fstream_.in | fstream_.trunc);
        isUsingDefautBuf = true;
    }

private:
    std::string fileName_;
    std::fstream fstream_;
    bool valid_ = false;
    void* defaultBuf_ = nullptr;
    bool isUsingDefautBuf = false;
    bool isUsingConstomizedBuf_ =false;
    static const std::streamsize DEFAULTBUFFERSIZE = 4*1024;
    uint64_t bytesWritten_ = 0;
};
}
