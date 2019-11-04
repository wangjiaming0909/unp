#!/bin/bash
cd gtests &&
#run tests
# ./test.out --gtest_filter=httpDownloader.*
./gtest.out

# ./test.out   --gtest_filter=TcpServer.*
# ./test.out   --gtest_filter=HttpParser.*
# ./test.out   --gtest_filter=TCPCLIENT.*

# ./test.out --gtest_filter=Http1xCodec.*