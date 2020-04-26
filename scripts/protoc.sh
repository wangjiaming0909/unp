#!/bin/bash

cd ~/codes/unp/scripts
protoc -I=../src/proto/ --cpp_out=../src/proto/ ../src/proto/mess_wl.proto && mv ../src/proto/mess_wl.pb.cc ../src/proto/mess_wl.pb.cpp
protoc -I=../src/proto/ --cpp_out=../src/proto/ ../src/proto/mess.proto && mv ../src/proto/mess.pb.cc ../src/proto/mess.pb.cpp
protoc -I=../examples/syncclient/ --cpp_out=../examples/syncclient ../examples/syncclient/sync_mess.proto && mv ../examples/syncclient/sync_mess.pb.cc ../examples/syncclient/sync_mess.pb.cpp
