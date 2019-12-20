#include "util/easylogging++.h"
#include "main_helper.h"
#include "downloader/download.h"
#include <string>
//#include "examples/Downloader.h"

// INITIALIZE_NULL_EASYLOGGINGPP
INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
    server_scoped_helper s_h{argc, argv};
//    examples::Downloader d{"https://github.com/wangjiaming0909/unp/archive/master.zip"};
//    d.getFileInfo();
//    d.download();
    if(argc < 2)
    {
        LOG(ERROR) << "a.out url";
        return -1;
    }
    std::string url = argv[1];
    downloader::Download d{url};
    int ret = d.download();
    std::cout << ret;
    return ret;
}
