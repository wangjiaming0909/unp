#!/bin/bash
cd gtests &&
#run tests
# ./test.out --gtest_filter=httpDownloader.*
# ./test.out --gtest_filter=HttpsClientHandler.*
./test.out

# ./test.out   --gtest_filter=TcpServer.*
# ./test.out   --gtest_filter=HttpParser.*
# ./test.out   --gtest_filter=TCPCLIENT.*

# ./test.out --gtest_filter=Http1xCodec.*