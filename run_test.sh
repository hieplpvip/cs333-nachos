#!/bin/bash
set -e

if [ $(arch) = "aarch64" ]; then
  export LD_LIBRARY_PATH="/usr/i686-linux-gnu/lib"
fi

cd code/test
make

echo "Running $1"
../build.linux/nachos -x "$@"
