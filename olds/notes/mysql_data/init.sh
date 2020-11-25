#!/bin/bash

$1/bin/mysqld --initialize-insecure --lower-case-table-names=1 --basedir=/home/wjm/loads/mysql8_data/$2 --datadir=/home/wjm/loads/mysql8_data/$2
