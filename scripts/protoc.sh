#!/bin/bash

cd ./scripts
protoc -I=../src/proto/ --cpp_out=../src/proto/ ../src/proto/sync_mess.proto \
&& mv ../src/proto/sync_mess.pb.cc ../src/proto/sync_mess.pb.cpp
