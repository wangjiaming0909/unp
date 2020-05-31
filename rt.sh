#!/bin/bash
#cd gtests &&
#run tests
# ./test.out --gtest_filter=httpDownloader.*
# ./test.out --gtest_filter=HttpsClientHandler.*
# ./test.out --gtest_filter=Downloader.GetFileInfo2
# ./test.out --gtest_filter=Downloader.DivideRanges
# ./test.out --gtest_filter=Downloader.GetFileInfoOfChunked
# ./test.out --gtest_filter=Download.normal
#./test.out --gtest_filter=proto2.*
#./test.out --gtest_filter=downloadserver.*

#./test.out --gtest_filter=DirMonitor.*


# ./test.out   --gtest_filter=TcpServer.*
# ./test.out   --gtest_filter=HttpParser.*
# ./test.out   --gtest_filter=TCPCLIENT.*

# ./test.out --gtest_filter=Http1xCodec.*


if [ $# = 0 ]
then
  cd gtests
  ./test.out
elif [ $1 = 1 ]
then
  ./build/examples/sync_client/sync_client
elif [ $1 = 2 ]
then
  ./build/examples/sync_server/sync_server
elif [ $# = 1 ]
then
  cd gtests
  ./test.out --gtest_filter=$1.*
elif [ $# = 2 ]
then
  cd gtests
  ./test.out --gtest_filter=$1.$2
fi

