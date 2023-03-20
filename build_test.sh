#!/bin/bash
set -e

if [ $(arch) = "aarch64" ]; then
  export LD_LIBRARY_PATH="/usr/i686-linux-gnu/lib"
fi

cd code/test
make

../build.linux/nachos -x halt
#../build.linux/nachos -x add
../build.linux/nachos -x openfile
../build.linux/nachos -x createfile
#../build.linux/nachos -x echoclient
