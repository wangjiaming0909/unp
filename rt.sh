#!/bin/bash
cd gtests &&
#run tests
# ./gtests/test.out --gtest_filter=httpDownloader.*

# ./gtests/test.out   --gtest_filter=TcpServer.*
# ./gtests/test.out   --gtest_filter=HttpParser.*
# ./gtests/test.out   --gtest_filter=TCPCLIENT.*

./test.out --gtest_filter=Http1xCodec.*