#include "downloadergetfileinfohandler.h"


namespace examples {

int DownloaderGetFileInfoHandler::onHeadersComplete(size_t len)
{

    isShouldClose_ = true;
    return 0;
}

int DownloaderGetFileInfoHandler::onBody(const char* buf, size_t len)
{

}
}
