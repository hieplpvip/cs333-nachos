#!/bin/bash
set -e

if [ $(arch) = "aarch64" ]; then
  export LD_LIBRARY_PATH="/usr/i686-linux-gnu/lib"
fi

cd code/test
make

../build.linux/nachos -x halt
