#include "net/file_stream.h"

namespace reactor
{
int FileStream::open()
{
}
int FileStream::close()
{
}
ssize_t FileStream::read(void *buffer, size_t len)
{
}

ssize_t FileStream::read(reactor::buffer &buf, size_t len)
{
}
ssize_t FileStream::write(const void *buffer, size_t len)
{
}

ssize_t FileStream::writev_n(const void *buffer, size_t len)
{
}
int FileStream::get_handle() const
{
}
bool FileStream::has_handle() const
{
}
int FileStream::setNonBolcking()
{
}
int FileStream::restoreBlocking()
{}

}
