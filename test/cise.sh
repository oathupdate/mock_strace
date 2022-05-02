#!/bin/bash

sh ../build.sh

./test_server &
sleep 1
pid=`pgrep -f test_server`
../strace $pid &
sleep 1
./test_client
