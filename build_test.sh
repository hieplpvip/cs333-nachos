#!/bin/bash
set -e

if [ $(arch) = "aarch64" ]; then
  export LD_LIBRARY_PATH="/usr/i686-linux-gnu/lib"
fi

cd code/test
make

../build.linux/nachos -x halt
../build.linux/nachos -x add
../build.linux/nachos -x createfile hello.txt
../build.linux/nachos -x delete hello.txt

../build.linux/nachos -x copy copy.c hello.c
../build.linux/nachos -x cat hello.c
../build.linux/nachos -x concatenate cat.c hello.c
../build.linux/nachos -x delete hello.c

../build.linux/nachos -x testExecJoin
../build.linux/nachos -x testSem false
../build.linux/nachos -x testSem true

#../build.linux/nachos -x echoclient
