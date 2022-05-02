#!/bin/bash

dir=$(cd $(dirname $0);pwd)
cd $dir
gcc main.c strace.c -o strace

cd test
gcc server.c -o test_server
gcc client.c -o test_client
