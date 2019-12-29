#!/bin/bash
cd gtests &&
#run tests
# ./test.out --gtest_filter=httpDownloader.*
# ./test.out --gtest_filter=HttpsClientHandler.*
# ./test.out --gtest_filter=Downloader.GetFileInfo2
# ./test.out --gtest_filter=Downloader.DivideRanges
# ./test.out --gtest_filter=Downloader.GetFileInfoOfChunked
# ./test.out --gtest_filter=Download.normal
./test.out --gtest_filter=proto2.*


# ./test.out   --gtest_filter=TcpServer.*
# ./test.out   --gtest_filter=HttpParser.*
# ./test.out   --gtest_filter=TCPCLIENT.*

# ./test.out --gtest_filter=Http1xCodec.*
