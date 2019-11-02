#!/bin/bash

touch /etc/ld.so.conf.d/lib64.conf && 
echo '/usr/local/lib64' > /etc/ld.so.conf.d/lib64.conf &&
ldconfig

