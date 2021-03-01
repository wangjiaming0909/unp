#!/bin/bash


$1/bin/mysqld \
  --defaults-file=/home/wjm/loads/mysql8_data/$2/my.cnf \
  --datadir=/home/wjm/loads/mysql8_data/$2 \
  --gdb --pid-file=./8/my.pid


#--daemonize
