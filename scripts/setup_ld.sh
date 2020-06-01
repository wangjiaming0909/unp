#!/bin/bash

touch /etc/ld.so.conf.d/lib64.conf &&
echo '/usr/local/lib64' > /etc/ld.so.conf.d/lib64.conf &&

touch /etc/ld.so.conf.d/boost.conf &&
echo '/boost_1_72_0/stage/lib' > /etc/ld.so.conf.d/boost.conf &&
ldconfig

