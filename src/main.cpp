#include "util/easylogging++.h"
#include "main_helper.h"
#include "examples/Downloader.h"

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
    examples::Downloader d{"https://github.com/wangjiaming0909/unp/archive/master.zip"};
    d.getFileInfo();
    d.download();
    server_scoped_helper s_h{argc, argv};
}
