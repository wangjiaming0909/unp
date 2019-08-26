#!/bin/bash
#run tests
# ./gtests/test.out   --gtest_filter=TcpServer.*
./gtests/test.out   --gtest_filter=HttpParser.*
