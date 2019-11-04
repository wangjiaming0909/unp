#include "Downloader.h"

namespace examples
{

Downloader::Downloader(const string_t& url)
    : url_{url}
{
}

Downloader::~Downloader()
{
}
}