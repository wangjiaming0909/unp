#include <gtest/gtest.h>
#include "downloader/download.h"

TEST(Download, normal)
{
    downloader::Download d{"https://github.com/wangjiaming0909/ace_fork/archive/master.zip"};
    auto ret = d.download();
    ASSERT_EQ(ret, 0);
}
