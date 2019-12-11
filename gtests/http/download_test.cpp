#include <gtest/gtest.h>
#include "downloader/download.h"

TEST(Download, normal)
{
    downloader::Download d{"https://github.com/wangjiaming0909/ace_fork/archive/master.zip"};
	//downloader::Download d{"http://d1.music.126.net/dmusic/netease-cloud-music_1.2.1_amd64_ubuntu_20190428.deb"};
	// downloader::Download d{"https://github.com/wangjiaming0909/unp/archive/master.zip"};
	// downloader::Download d{"http://fastsoft.onlinedown.net/down/Wechat_PCDownload1100109931.exe"};
    auto ret = d.download();
    ASSERT_EQ(ret, 0);
}
